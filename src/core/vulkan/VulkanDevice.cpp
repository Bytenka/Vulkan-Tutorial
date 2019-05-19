#include "pch.hpp"

#include "VulkanDevice.hpp"

VulkanDevice::VulkanDevice(VkInstance instance)
{
    try {
        selectPhysicalDevice(instance);  // Sets m_physicalDevice

        // Debug / logging stuff
        VkPhysicalDeviceProperties debugDP;
        vkGetPhysicalDeviceProperties(m_physicalDevice, &debugDP);
        LOG_TRACE("Picked up physical device \"{}\" for rendering", debugDP.deviceName);

        LOG_TRACE("Initialized Vulkan device");

    } catch (const Exception& ex) {
        LOG_ERROR("Failed to initialize Vulkan device");
        throw;
    }
}

VulkanDevice::~VulkanDevice()
{
    LOG_TRACE("Destroying Vulkan device");
}

// public

// private

void VulkanDevice::selectPhysicalDevice(VkInstance instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        throw Exception("No GPU found with Vulkan support");

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    // Request the more suitable device from that list
    m_physicalDevice = getBestPhysicalDevice(physicalDevices);

    if (m_physicalDevice == VK_NULL_HANDLE)
        throw Exception("No suitable GPU found");
}


VkPhysicalDevice VulkanDevice::getBestPhysicalDevice(const std::vector<VkPhysicalDevice>& physicalDevices) const noexcept
{
    std::multimap<unsigned long long, VkPhysicalDevice> sortedDevices;

    for (const auto& device : physicalDevices) {
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
        if (!getPhysicalDeviceQueueFamilyIndices(device).isComplete()) score = 0;

        sortedDevices.insert({score, device});
    }

    auto it = sortedDevices.rbegin();  // Valid if !sortedDevices.empty()
    return !sortedDevices.empty() && it->first > 0 ? it->second : VK_NULL_HANDLE;
}


VulkanDevice::QueueFamilyIndices VulkanDevice::getPhysicalDeviceQueueFamilyIndices(VkPhysicalDevice physicalDevice) const noexcept
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t index = 0;
    for (auto& q : queueFamilies) {
        if (q.queueCount > 0) {
            if (q.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphicsFamilyIndex = index;
        }

        if (indices.isComplete())
            break;

        ++index;
    }

    return indices;
}
