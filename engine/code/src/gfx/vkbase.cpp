#include "anthraxAI/gfx/vkbase.h"

void Gfx::Vulkan::Init()
{
    CreateVkInstance();

    Gfx::Device::GetInstance()->Init();
	
	Gfx::Renderer::GetInstance()->CreateCommands();
	Gfx::Renderer::GetInstance()->CreateRenderTargets();
	Gfx::Renderer::GetInstance()->Sync();
	Gfx::Renderer::GetInstance()->CreateTextures();

	Gfx::DescriptorsBase::GetInstance()->Init();
	Gfx::DescriptorsBase::GetInstance()->AllocateBuffers();
	Gfx::Pipeline::GetInstance()->Build();
	Gfx::Mesh::GetInstance()->CreateMeshes();
	Gfx::Model::GetInstance()->LoadModels();
	
}

bool Gfx::Vulkan::ReloadShaders()
{
    vkDeviceWaitIdle(Gfx::Device::GetInstance()->GetDevice());
    Core::PipelineDeletor::GetInstance()->CleanAll();

	Gfx::Pipeline::GetInstance()->Build();

	Core::Scene::GetInstance()->UpdateMaterials();

	return true;
}

bool Gfx::Vulkan::OnResize()
{
    if (Gfx::Renderer::GetInstance()->IsOnResize() && Core::WindowManager::GetInstance()->GetScreenResolution().x > 0 && Core::WindowManager::GetInstance()->GetScreenResolution().y > 0) {
    printf("00001\n\n");
	    Gfx::Device::GetInstance()->RecreateSwapchain();
    printf("00002\n\n");

        Gfx::Renderer::GetInstance()->CreateRenderTargets();
    printf("00003\n\n");

        Gfx::Pipeline::GetInstance()->Build();
    printf("00004\n\n");

        return true;
    }
    return false;
}

void Gfx::Vulkan::CleanUp()
{
    vkDeviceWaitIdle(Gfx::Device::GetInstance()->GetDevice());
	for (Gfx::FrameData frame : Gfx::Renderer::GetInstance()->Frames) {
        vkWaitForFences(Gfx::Device::GetInstance()->GetDevice(), 1, &frame.RenderFence, true, 1000000000);
    }

	Gfx::Mesh::GetInstance()->CleanAll();
	Gfx::Model::GetInstance()->CleanAll();

    Core::PipelineDeletor::GetInstance()->CleanAll();
	Gfx::Renderer::GetInstance()->CleanResources();
	Gfx::DescriptorsBase::GetInstance()->CleanAll();
	Gfx::DescriptorsBase::GetInstance()->CleanBindless();

	Gfx::Device::GetInstance()->CleanUpSwapchain();
	
	Core::Deletor::GetInstance()->CleanAll();

	vkDestroySurfaceKHR(Instance, Gfx::Device::GetInstance()->GetSurface(), nullptr);
	vkDestroyDevice(Gfx::Device::GetInstance()->GetDevice(), nullptr);
	vkDestroyInstance(Instance, nullptr);
}

void Gfx::Vulkan::CreateVkInstance()
{
    VkApplicationInfo appinfo{};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pApplicationName = "MyAppName";
    appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.pEngineName = "MyEngine";
    appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createinfo{};
	createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createinfo.pApplicationInfo = &appinfo;

	if (!INSTANCE_EXT.empty()) {
		ASSERT(!EnumerateInstanceExtSupport(), "Not supported required instance extensions!");
		createinfo.ppEnabledExtensionNames = INSTANCE_EXT.data();
		createinfo.enabledExtensionCount = static_cast<uint32_t>(INSTANCE_EXT.size());
	}

	ASSERT((ValidationLayersOn && !EnumerateValidationLayerSupport()), "Not supported validation layers!");

	if (ValidationLayersOn) {
		createinfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYER.size());
		createinfo.ppEnabledLayerNames = VALIDATION_LAYER.data();
        createinfo.pNext = Debug.GetInfo();
	}
	else {
		createinfo.enabledLayerCount = 0;
        createinfo.pNext = nullptr;
	}

	VK_ASSERT(vkCreateInstance(&createinfo, nullptr, &Instance), "vkCreateInstance failed");

	Debug.Setup(Instance, ValidationLayersOn);

	Core::Deletor::GetInstance()->Push([=, this]() {
		if (ValidationLayersOn) {
			Debug.Destroy(Instance, nullptr);
		}
	});
}

bool Gfx::Vulkan::EnumerateInstanceExtSupport()
{
	unsigned int instextcount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &instextcount, nullptr);
	
	std::vector<VkExtensionProperties> availableextensions(instextcount);
	vkEnumerateInstanceExtensionProperties(nullptr, &instextcount, availableextensions.data());

	for (const char *requiredextname : INSTANCE_EXT) {
		bool found = false;
		for (const VkExtensionProperties &extproperties : availableextensions) {
			if (strcmp(requiredextname, extproperties.extensionName) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			return false;
		}
	}
	return true;
}

bool Gfx::Vulkan::EnumerateValidationLayerSupport()
{
    uint32_t layercount;
	vkEnumerateInstanceLayerProperties(&layercount, nullptr);

	std::vector<VkLayerProperties> availablelayers(layercount);
	vkEnumerateInstanceLayerProperties(&layercount, availablelayers.data());

	for (const char* layername : VALIDATION_LAYER) {
	    bool found = false;

	    for (const auto& layerproperties : availablelayers) {
	        if (strcmp(layername, layerproperties.layerName) == 0) {
	            found = true;
	            break;
	        }
	    }
	    if (!found) {
	        return false;
	    }
	}
	return true;
}