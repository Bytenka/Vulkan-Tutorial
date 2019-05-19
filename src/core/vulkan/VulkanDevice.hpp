#pragma once

#include <vulkan/vulkan.hpp>

class VulkanDevice
{
  public:
    VulkanDevice(VkInstance instance);
    ~VulkanDevice();

  private:
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamilyIndex;
        inline bool isComplete() const noexcept { return graphicsFamilyIndex.has_value(); }
    };

  private:
    void selectPhysicalDevice(VkInstance instance);

    VkPhysicalDevice getBestPhysicalDevice(const std::vector<VkPhysicalDevice>& physicalDevices) const noexcept;
    QueueFamilyIndices getPhysicalDeviceQueueFamilyIndices(VkPhysicalDevice physicalDevice) const noexcept;

  private:
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_logicalDevice = VK_NULL_HANDLE;

  private:
  public:
    VulkanDevice(const VulkanDevice&) = delete;
    void operator=(const VulkanDevice&) = delete;
};