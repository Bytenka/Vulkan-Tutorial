#pragma once

#include <vulkan/vulkan.hpp>

#include <optional>
#include <memory>

class VulkanDevice;
class VulkanInstance
{
  public:
    VulkanInstance();
    ~VulkanInstance();

  private:
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamilyIndex;
        inline bool isComplete() const noexcept { return graphicsFamilyIndex.has_value(); }
    };

  private:
    void setupDebugMessenger();
    void populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const noexcept;
    void destroyDebugMessenger() noexcept;

    const std::vector<const char*> getValidationLayers() const noexcept;
    const std::vector<const char*> getRequiredExtensions() const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugVLCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

  private:
    VkInstance m_vkInstance;
    std::unique_ptr<VulkanDevice> m_vkDevice;

    VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;
    bool m_usingValidationLayers;

  public:
    VulkanInstance(const VulkanInstance&) = delete;
    void operator=(const VulkanInstance&) = delete;
};