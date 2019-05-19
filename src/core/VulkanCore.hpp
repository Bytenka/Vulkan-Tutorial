#pragma once

#include <vulkan/vulkan.hpp>

class VulkanCore
{
  public:
    VulkanCore();
    ~VulkanCore();

  private:
    void setupPhysicalDevice();
    VkPhysicalDevice pickPhysicalDevice(std::vector<VkPhysicalDevice>& devicesList) const noexcept;

    const std::vector<const char*> getValidationLayers() const noexcept;
    const std::vector<const char*> getRequiredExtensions() const;

  private:
    VkInstance m_vkInstance;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    bool m_usingValidationLayers;

  private:  // Vulkan validation layer / debug stuff
    VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugVLCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    void populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const noexcept;
    void setupDebugMessenger();
    void destroyDebugMessenger() noexcept;

  public:
    VulkanCore(const VulkanCore&) = delete;
    void operator=(const VulkanCore&) = delete;
};