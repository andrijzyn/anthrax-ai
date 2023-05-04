#pragma once

#include "vkdefines.h"
#include <optional>
#include <set>
#include <algorithm>

struct QueueBuilder {
	VkQueue graphicsqueue;
	VkQueue presentqueue;
};

struct QueueFamilyIndex {
	std::optional<uint32_t> graphicsfamily;
	std::optional<uint32_t> presentfamily;

	bool iscomplete() {
		return graphicsfamily.has_value()
		&& presentfamily.has_value();
	}
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR		capabilities;
    std::vector<VkSurfaceFormatKHR>	formats;
    std::vector<VkPresentModeKHR>	presentmodes;
};

class DeviceBuilder {
public:
#ifdef OS_WINDOWS
	void 						init(HWND& hwndh, VkInstance& instanceh, VkSurfaceKHR& surfaceh, DeletionQueue&	deletor) 
								{ hwnd = hwndh; instance = instanceh; surface = surfaceh; deletorhandler = deletor; };
#endif
#ifdef OS_LINUX
	void 						init(VkInstance& instanceh, VkSurfaceKHR& surfaceh, DeletionQueue&	deletor) 
								{ instance = instanceh; surface = surfaceh; deletorhandler = deletor; };
#endif

	QueueFamilyIndex			findqueuefamilies(VkPhysicalDevice& device);
	SwapChainSupportDetails 	queryswapchainsupport(VkPhysicalDevice& device);
	bool						deviceextensionssupport(VkPhysicalDevice& device);
	bool						isdevisesuitable(VkPhysicalDevice device);

	void 						findphysicaldevice();
	void 						buildlogicaldevice();
	void 						buildswapchain();
	void 						buildimagesview();

	VkExtent2D 					chooseswapextent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkPhysicalDevice&			getphysicaldevice()			{ return physicaldevbuilder;	};
	VkDevice&					getlogicaldevice()			{ return logicaldevbuilder;		};
	VkSwapchainKHR&				getswapchain()				{ return swapchainbuilder;		};
	VkFormat&					getswapchainformat()		{ return swapchainimgformatbuilder;};
	std::vector<VkImage>&		getswapchainimage()			{ return swapchainimgbuilder;	};
	std::vector<VkImageView>&	getswapchainimageview()		{ return swapchainimgviewsbuilder;};
	VkExtent2D&					getswapchainextent()		{ return swapchainextentbuilder;};
	QueueBuilder& 				getqueue()					{ return queue;					};
	VkSurfaceKHR&				getsurface()				{ return surface;				};

	size_t 						minuniformbufferoffsetalignment;


private:
#ifdef OS_WINDOWS
	HWND 						hwnd;
#endif
	DeletionQueue				deletorhandler;
	VkInstance					instance;
	VkSurfaceKHR 				surface;
	VkPhysicalDevice 			physicaldevbuilder;
	VkDevice					logicaldevbuilder;
	VkSwapchainKHR				swapchainbuilder;
	VkFormat 					swapchainimgformatbuilder;
	std::vector<VkImage>		swapchainimgbuilder;
	std::vector<VkImageView> 	swapchainimgviewsbuilder;
	VkExtent2D 					swapchainextentbuilder;


	QueueBuilder				queue;
};
