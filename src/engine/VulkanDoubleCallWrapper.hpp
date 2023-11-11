#ifndef VULKAN_DOUBLE_CALL_WRAPPER_HPP
#define VULKAN_DOUBLE_CALL_WRAPPER_HPP

#include <vulkan/vulkan.h>

#include <tuple>
#include <vector>

namespace engine {

template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> {
  using last_argument_type = typename std::
      tuple_element<sizeof...(Args) - 1, std::tuple<Args...>>::type;
};

template <typename F, typename... Args>
/**
 * This is a helper wrapper function to avoid the doubled calls for some
 * vulkan functions like `vkEnumerateDeviceExtensionProperties`.
 * Instead of repeating this behaviour:
 * \code
 * uint32_t extensionCount;
 * vkEnumerateDeviceExtensionProperties(
 *     device, nullptr, &extensionCount, nullptr
 * );
 * std::vector<VkExtensionProperties> availableExtensions(extensionCount);
 * vkEnumerateDeviceExtensionProperties(
 *     device, nullptr, &extensionCount, availableExtensions.data()
 * );
 * \endcode
 *
 * You can just do this:
 *
 * \code
 * // The vector type will be inferred to std::vector<VkExtensionProperties>
 * std::vector availableExtensions =
 *     vkCall(
 *         vkEnumerateDeviceExtensionProperties,
 *         physicalDevice,
 *         pLayerName);
 * \endcode
 * @tparam F
 * @tparam Args
 * @param func the vkCreate function to be called
 * @param args all arguments of vkCreate except last two
 * @return
 */
auto vkCall(F func, Args&&... args) -> std::vector<typename std::remove_pointer<
    typename function_traits<F>::last_argument_type>::type> {
  using T = typename std::remove_pointer<
      typename function_traits<F>::last_argument_type>::type;

  uint32_t count;
  func(std::forward<Args>(args)..., &count, nullptr);

  std::vector<T> container(count);
  func(std::forward<Args>(args)..., &count, container.data());

  return container;
}

}  // namespace engine


#endif  // VULKAN_DOUBLE_CALL_WRAPPER_HPP
