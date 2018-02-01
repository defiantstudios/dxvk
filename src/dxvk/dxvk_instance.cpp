#include "dxvk_instance.h"

namespace dxvk {

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData) {

        Logger::debug(msg);

        return VK_FALSE;
    }
  
  DxvkInstance::DxvkInstance()
  : m_vkl(new vk::LibraryFn()),
    m_vki(new vk::InstanceFn(this->createInstance())) {
    
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = debugCallback;
    m_vki->vkCreateDebugReportCallbackEXT(m_vki->instance(), &createInfo, nullptr, &m_drc);
  }
  
  
  DxvkInstance::~DxvkInstance() {
     m_vki->vkDestroyDebugReportCallbackEXT(m_vki->instance(), m_drc, nullptr);
  }
  
  
  std::vector<Rc<DxvkAdapter>> DxvkInstance::enumAdapters() {
    uint32_t numAdapters = 0;
    if (m_vki->vkEnumeratePhysicalDevices(m_vki->instance(), &numAdapters, nullptr) != VK_SUCCESS)
      throw DxvkError("DxvkInstance::enumAdapters: Failed to enumerate adapters");
    
    std::vector<VkPhysicalDevice> adapters(numAdapters);
    if (m_vki->vkEnumeratePhysicalDevices(m_vki->instance(), &numAdapters, adapters.data()) != VK_SUCCESS)
      throw DxvkError("DxvkInstance::enumAdapters: Failed to enumerate adapters");
    
    std::vector<Rc<DxvkAdapter>> result;
    for (uint32_t i = 0; i < numAdapters; i++)
      result.push_back(new DxvkAdapter(this, adapters[i]));
    return result;
  }
  
  
  VkInstance DxvkInstance::createInstance() {
    auto enabledLayers     = this->getLayers();
    auto enabledExtensions = this->getExtensions(enabledLayers);
    
    Logger::info("Enabled instance layers:");
    this->logNameList(enabledLayers);
    Logger::info("Enabled instance extensions:");
    this->logNameList(enabledExtensions);
    
    VkApplicationInfo appInfo;
    appInfo.sType                 = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext                 = nullptr;
    appInfo.pApplicationName      = nullptr;
    appInfo.applicationVersion    = 0;
    appInfo.pEngineName           = "DXVK";
    appInfo.engineVersion         = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion            = VK_MAKE_VERSION(1, 0, 47);
    
    VkInstanceCreateInfo info;
    info.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext                    = nullptr;
    info.flags                    = 0;
    info.pApplicationInfo         = &appInfo;
    info.enabledLayerCount        = enabledLayers.count();
    info.ppEnabledLayerNames      = enabledLayers.names();
    info.enabledExtensionCount    = enabledExtensions.count();
    info.ppEnabledExtensionNames  = enabledExtensions.names();
    
    VkInstance result = VK_NULL_HANDLE;
    if (m_vkl->vkCreateInstance(&info, nullptr, &result) != VK_SUCCESS)
      throw DxvkError("DxvkInstance::createInstance: Failed to create Vulkan instance");

	return result;
  }
  
  
  vk::NameList DxvkInstance::getLayers() {
    std::vector<const char*> layers = { };
    
    if (env::getEnvVar(L"DXVK_DEBUG_LAYERS") == "1") {
      layers.push_back("VK_LAYER_LUNARG_standard_validation");
    }
    
    const vk::NameSet layersAvailable
      = vk::NameSet::enumerateInstanceLayers(*m_vkl);
    
    vk::NameList layersEnabled;
    for (auto l : layers) {
      if (layersAvailable.supports(l))
        layersEnabled.add(l);
    }
    
    return layersEnabled;
  }
  
  
  vk::NameList DxvkInstance::getExtensions(const vk::NameList& layers) {
    std::vector<const char*> extOptional = { };
    std::vector<const char*> extRequired = {
      VK_KHR_SURFACE_EXTENSION_NAME,
      VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    };

	if (env::getEnvVar(L"DXVK_DEBUG_LAYERS") == "1") {
      extOptional.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
    
    const vk::NameSet extensionsAvailable
      = vk::NameSet::enumerateInstanceExtensions(*m_vkl, layers);
    vk::NameList extensionsEnabled;
    
    for (auto e : extOptional) {
      if (extensionsAvailable.supports(e)) 
        extensionsEnabled.add(e);
    }
    
    for (auto e : extRequired) {
      if (!extensionsAvailable.supports(e))
        throw DxvkError(str::format("DxvkInstance::getExtensions: Extension ", e, " not supported"));
      extensionsEnabled.add(e);
    }
    
    return extensionsEnabled;
  }
  
  
  void DxvkInstance::logNameList(const vk::NameList& names) {
    for (uint32_t i = 0; i < names.count(); i++)
      Logger::info(str::format("  ", names.name(i)));
  }
  
}
