#include "pch.hpp"

#include "VulkanCore.hpp"
#include "core/Logger.hpp"

#include <vulkan/vulkan.hpp>

#include <algorithm>
#include <map>

VulkanCore::VulkanCore()
{
#ifdef NDEBUG
    m_usingValidationLayers = false;
#else
    m_usingValidationLayers = true;
#endif

    if (!glfwVulkanSupported())
        throw Exception("Vulkan is not available on this machine");

    VkApplicationInfo appInfo = {};  // Default everything to 0
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = nullptr;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = nullptr;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;


    // Get required extensions
    auto requiredExtensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();


    // Get validation layer
    std::vector<const char*> validationLayers;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (m_usingValidationLayers) {
        validationLayers = getValidationLayers();
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessenger(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }


    // Get supported extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);  // Set extensionCount
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());  // Use extentionCount


    // Check if every required extension is supported
    std::vector<VkExtensionProperties> unsupportedExtensions;
    for (unsigned i = 0; i < createInfo.enabledExtensionCount; i++) {
        auto it = std::find_if(
            extensions.begin(),
            extensions.end(),
            [&](const VkExtensionProperties& ext) {
                return std::strcmp(createInfo.ppEnabledExtensionNames[i], ext.extensionName) == 0;
            });

        if (it == extensions.end())
            unsupportedExtensions.push_back(extensions[i]);
    }

    if (!unsupportedExtensions.empty()) {
        std::string msg = "The following required Vulkan extensions are not supported:";
        for (auto& ext : unsupportedExtensions)
            msg += " " + std::string(ext.extensionName);

        throw Exception(msg);
    }


    // Finally create the instance
    if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS)
        throw Exception("Failed to create Vulkan instance");


    // After that, we can setup the debug callback
    if (m_usingValidationLayers) {
        try {
            setupDebugMessenger();
        } catch (const Exception& ex) {
            LOG_ERROR("Failed to set up debug messenger\n{}", ex.what());
        }
    }

    LOG_TRACE("Initialized Vulkan instance");

    setupPhysicalDevice();
}

VulkanCore::~VulkanCore()
{
    LOG_TRACE("Destroying Vulkan instance");

    if (m_usingValidationLayers)
        destroyDebugMessenger();

    vkDestroyInstance(m_vkInstance, nullptr);
}

// public

// private

void VulkanCore::setupPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
        throw Exception("No GPU found with Vulkan support");

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, physicalDevices.data());

    m_physicalDevice = pickPhysicalDevice(physicalDevices);

    if (m_physicalDevice == VK_NULL_HANDLE)
        throw Exception("No suitable GPU found");


    VkPhysicalDeviceProperties debugDP;
    vkGetPhysicalDeviceProperties(m_physicalDevice, &debugDP);
    LOG_TRACE("Picked up physical device \"{}\" for rendering", debugDP.deviceName);
}


VkPhysicalDevice VulkanCore::pickPhysicalDevice(std::vector<VkPhysicalDevice>& devicesList) const noexcept
{
    std::multimap<unsigned long long, VkPhysicalDevice> sortedDevices;

    for (auto& device : devicesList) {
        VkPhysicalDeviceProperties dp;
        VkPhysicalDeviceFeatures df;

        vkGetPhysicalDeviceProperties(device, &dp);
        vkGetPhysicalDeviceFeatures(device, &df);

        unsigned long long score = 0;
        // Add other criterias below
        score += dp.limits.maxImageDimension2D;
        if (dp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
        
        // Add mandatory criterias below
        if (!df.geometryShader) score = 0;

        sortedDevices.insert({score, device});
    }

    auto it = sortedDevices.rbegin();  // Valid if !sortedDevices.empty()
    return !sortedDevices.empty() && it->first > 0 ? it->second : VK_NULL_HANDLE;
}


const std::vector<const char*> VulkanCore::getValidationLayers() const noexcept
{
    std::vector<const char*> usingValidationLayers = {
        "VK_LAYER_KHRONOS_validation"};


    // Check layers validity/availability
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    std::string removedLayers;
    for (int i = usingValidationLayers.size() - 1; i >= 0; i--) {
        auto it = std::find_if(
            availableLayers.begin(),
            availableLayers.end(),
            [&](const VkLayerProperties& avLayer) {
                return std::strcmp(usingValidationLayers[i], avLayer.layerName) == 0;
            });

        if (it == availableLayers.end()) {
            removedLayers += " " + std::string(usingValidationLayers[i]);
            usingValidationLayers.erase(usingValidationLayers.begin() + i);
        }
    }

    if (!removedLayers.empty()) {
        LOG_WARN(
            "The following Vulkan validation layers are invalid or unsupported and were removed (see {}):{}",
            __FUNCTION__,
            removedLayers);
    }


    return usingValidationLayers;
}


const std::vector<const char*> VulkanCore::getRequiredExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    if (glfwExtensions == NULL)
        throw Exception("No Vulkan extensions for window surface creation. Application can't draw to screen");

    std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_usingValidationLayers)
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return requiredExtensions;
}


VKAPI_ATTR VkBool32 VKAPI_CALL VulkanCore::debugVLCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    // pUserData is "this" for now

    switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        //LOG_DEBUG("[Vulkan] ", pCallbackData->pMessage);
        LOG_DEBUG("[Vulkan API Debug] {}", pCallbackData->pMessage);
        break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        //LOG_INFO(pCallbackData->pMessage);
        LOG_DEBUG("[Vulkan API Info] {}", pCallbackData->pMessage);
        break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        //LOG_WARN(pCallbackData->pMessage);
        LOG_DEBUG("[Vulkan API Warn] {}", pCallbackData->pMessage);
        break;

    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        //LOG_ERROR(pCallbackData->pMessage);
        LOG_DEBUG("[Vulkan API Error] {}", pCallbackData->pMessage);
        break;

    default:
        break;
    }

    return VK_FALSE;
}


void VulkanCore::populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const noexcept
{
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = VulkanCore::debugVLCallback;
    createInfo.pUserData = (void*)this;  // Passing this instance in case we need it. Unused for now
}


void VulkanCore::setupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    populateDebugMessenger(createInfo);

    // Query for vkCreateDebugUtilsMessengerEXT, because it's an extension
    auto createDebugUtilsMessengerEXT =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            m_vkInstance,
            "vkCreateDebugUtilsMessengerEXT");


    if (createDebugUtilsMessengerEXT == nullptr)
        throw Exception("vkCreateDebugUtilsMessengerEXT unavailable, extension not loaded");

    if (createDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
        throw Exception("Failed to create VkDebugUtilsMessengerEXT");
}


void VulkanCore::destroyDebugMessenger() noexcept
{
    // Query for vkDestroyDebugUtilsMessengerEXT, because it's an extension
    auto destroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            m_vkInstance,
            "vkDestroyDebugUtilsMessengerEXT");


    if (destroyDebugUtilsMessengerEXT != nullptr && m_debugMessenger != nullptr)
        destroyDebugUtilsMessengerEXT(m_vkInstance, m_debugMessenger, nullptr);

    m_debugMessenger = nullptr;
}