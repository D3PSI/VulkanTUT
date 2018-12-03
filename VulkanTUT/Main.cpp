/*
*	Main.cpp: Defines the entry point for the console application.
*
*/
#define VK_USE_PLATFORM_WIN32_KHR
#include "Logger.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
//#include "vulkan/vulkan.h"
#include <iostream>
#include <intrin.h>
#include <vector>
#include <thread>

/*
*	Makro:			ASSERT_VULKAN(val)
*	Purpose:		Checks for error codes and pauses code execution if necessary
*
*/
#define ASSERT_VULKAN(val)\
	\
	if(val != VK_SUCCESS) {\
	\
		__debugbreak();\
	\
	}

/*
*	Prototypes
*
*/
namespace game {

	namespace vulkan {

		void init(void);
		void deviceProperties(VkPhysicalDevice &device);
		void deviceFeatures(VkPhysicalDevice &device);
		void deviceMemoryProperties(VkPhysicalDevice &device);
		void queueFamilyProperties(VkPhysicalDevice &device);
		void deviceQueueCreateInfos(VkPhysicalDevice &device);
		void deviceCreateInfo();
		void device(void);
		void createQueue(void);
		void createSurface(void);
		void surfaceCapabilities(VkPhysicalDevice &device);
		void swapchainCreate(void);
		void shutdownVulkan(void);		
		void drawFrame();
		void createShaderModule(const std::vector< char >& code, VkShaderModule* shaderModule);
		std::vector< char > readFile(const std::string &filename);

	}

	namespace glfw {

		void init(void);
		void gameLoop(void);
		void shutdownGLFW(void);

	}

}

/*
*	Namespace:		game
*	Purpose:		Prevent global naming conflicts
*
*/
namespace game {

	/*
	*	Global Variables
	*
	*/
	VkResult										result;

	GLFWwindow*										window;

	VkInstance										instance;
	VkDevice										logicalDevice;
	VkSurfaceKHR									surface;
	VkSwapchainKHR									swapchain;
	VkImageView*									imageViews;
	VkFramebuffer*									framebuffers;
	VkCommandPool									commandPool;
	VkQueue											queue;
	VkCommandBuffer*								commandBuffers;
	VkPipelineLayout								pipelineLayout;
	VkPipeline										pipeline;
	VkRenderPass									renderPass;
	VkSemaphore										semaphoreImageAvailable;
	VkSemaphore										semaphoreRenderingFinished;
	VkViewport										viewport;

	const unsigned int WINDOW_WIDTH					= 1280;
	const unsigned int WINDOW_HEIGHT				= 780;
	const char* TITLE								= "D3PSI's first VULKAN engine";
	const VkFormat colorAttachmentFormat			= VK_FORMAT_B8G8R8A8_UNORM;		// TODO: Check if valid


	/*
	*	Namespace:		vulkan
	*	Purpose:		Differentiate between GLFW and VULKAN
	*
	*/
	namespace vulkan {

		/*
		*	Global Variables in namespace
		*
		*/
		Logger										logger;
		VkPhysicalDevice*							physicalDevices;
		VkLayerProperties*							layers;
		VkExtensionProperties*						extensions;

		uint32_t amountOfImagesInSwapchain			= 0;
			   
		VkShaderModule shaderModuleVert;
		VkShaderModule shaderModuleFrag;

		/*
		*	Function:		void vulkan::init()
		*	Purpose:		Initializes the Vulkan API
		*
		*/
		void init() {

			logger.start();
			logger.log(START_STOP_LOG, "Startup initialized...");

			// Application info
			VkApplicationInfo appInfo;
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pNext = NULL;
			appInfo.pApplicationName = "VULKAN TUTORIAL";
			appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
			appInfo.pEngineName = "Tutorial Vulkan Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_1;

			logger.log(EVENT_LOG, "VkApplicationInfo gathered");

			uint32_t amountOfLayers = 0;
			vkEnumerateInstanceLayerProperties(&amountOfLayers, NULL);
			layers = new VkLayerProperties[amountOfLayers];
			vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);

			std::cout << "Amount of instance layers:	" << amountOfLayers << std::endl;
			for (unsigned int i = 0; i < amountOfLayers; i++) {

				std::cout << "Name:				" << layers[i].layerName					<< std::endl;
				std::cout << "Spec-Version:		" << layers[i].specVersion					<< std::endl;
				std::cout << "Impl-Version:		" << layers[i].implementationVersion		<< std::endl;
				std::cout << "Description:		" << layers[i].description					<< std::endl;
				std::cout << "------------------"											<< std::endl;

			}

			uint32_t amountOfExtensions = 0;
			vkEnumerateInstanceExtensionProperties(

				NULL,
				&amountOfExtensions,
				NULL

			);
			extensions = new VkExtensionProperties[amountOfExtensions];
			vkEnumerateInstanceExtensionProperties(

				NULL,
				&amountOfExtensions,
				extensions

			);

			std::cout << "Amount of extensions:	" << amountOfExtensions << std::endl;
			for (unsigned int i = 0; i < amountOfExtensions; i++) {

				std::cout << "Name:	"				<< extensions[i].extensionName		<< std::endl;
				std::cout << "Spec-Version:	"		<< extensions[i].specVersion		<< std::endl;
				std::cout << "------------------"	<< std::endl;

			}

			const std::vector< const char* > validationLayers = {

				"VK_LAYER_LUNARG_standard_validation"

			};

			uint32_t amountOfGlfwExtensions = 0;
			auto glfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);

			/*const std::vector< const char* > usedExtensions = {

				"VK_KHR_surface",
				VK_KHR_SURFACE_EXTENSION_NAME

			};*/

			// Instance info
			VkInstanceCreateInfo instanceInfo;
			instanceInfo.sType							= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceInfo.pNext							= NULL;
			instanceInfo.flags							= 0;
			instanceInfo.pApplicationInfo				= &appInfo;
			instanceInfo.enabledLayerCount				= validationLayers.size();
			instanceInfo.ppEnabledLayerNames			= validationLayers.data();
			instanceInfo.enabledExtensionCount			= amountOfGlfwExtensions;
			instanceInfo.ppEnabledExtensionNames		= glfwExtensions;

			logger.log(EVENT_LOG, "VkInstanceCreateInfo gathered");

			// Instance creation
			result = vkCreateInstance(

				&instanceInfo,	// Pass instance info
				NULL,			// Pass no allocation callback
				&instance		// Pass the actual instance

			);
			ASSERT_VULKAN(result);

			logger.log(EVENT_LOG, "Instance created successfully");

			// Surface creation
			result = glfwCreateWindowSurface(

				instance,		// Pass instance
				window,			// Pass the window
				nullptr,		// We do not want to use our own allocator
				&surface		// Pass the actual surface itself

			);
			ASSERT_VULKAN(result);

			// Enumerate GPU's (physically)
			uint32_t amountOfPhysicalDevices = 0;
			result = vkEnumeratePhysicalDevices(

				instance,						// Pass the instance
				&amountOfPhysicalDevices,		// Pass the variable for the amount of GPU's
				NULL							// Pass no array to store them, so we can just get the number of GPU's

			);
			ASSERT_VULKAN(result);

			physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];
			result = vkEnumeratePhysicalDevices(

				instance,						// Pass the instance
				&amountOfPhysicalDevices,		// Pass the amount of GPU's
				physicalDevices					// Pass the array to store the enumerated GPU's

			);
			ASSERT_VULKAN(result);

			std::cout << "Number of GPU's:	" << amountOfPhysicalDevices << std::endl;
			for (unsigned int i = 0; i < amountOfPhysicalDevices; i++) {

				deviceProperties(physicalDevices[i]);
				deviceFeatures(physicalDevices[i]);
				deviceMemoryProperties(physicalDevices[i]);
				queueFamilyProperties(physicalDevices[i]);
				deviceQueueCreateInfos(physicalDevices[i]);
				surfaceCapabilities(physicalDevices[i]);

			}

			deviceCreateInfo();
			swapchainCreate();

		}

		/*
		*	Function:		void vulkan::deviceProperties(vkPhysicalDevice &device)
		*	Purpose:		Prints the device information for every GPU
		*
		*/
		void deviceProperties(VkPhysicalDevice &device) {

			// Device properties
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);

			uint32_t apiVer = properties.apiVersion;

			std::cout << "Name:	" << properties.deviceName << std::endl;
			std::cout << "API-Version:	" << VK_VERSION_MAJOR(apiVer) <<
				"." << VK_VERSION_MINOR(apiVer) << "." <<
				VK_VERSION_PATCH(apiVer) << std::endl;
			std::cout << "Discrete-Queue-Priorities:	" << properties.limits.discreteQueuePriorities		<< std::endl;
			std::cout << "Driver-Version:				" << properties.driverVersion						<< std::endl;
			std::cout << "Vendor-ID:					" << properties.vendorID							<< std::endl;
			std::cout << "Device-ID:					" << properties.deviceID							<< std::endl;
			std::cout << "Device-Type:					" << properties.deviceType							<< std::endl;
			std::cout << "Pipeline-Cache-UUID:			" << properties.pipelineCacheUUID					<< std::endl;
			std::cout << "------------------------------"													<< std::endl;

		}

		/*
		*	Function:		void vulkan::deviceFeatures(VkPhysicalDevice &device)
		*	Purpose:		Prints detailed information about the GPU
		*
		*/
		void deviceFeatures(VkPhysicalDevice &device) {

			// Device features
			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(device, &features);

			std::cout << "Features:											"															<< std::endl;
			std::cout << "\tRobust Buffer Access:								" << features.robustBufferAccess							<< std::endl;
			std::cout << "\tFull Draw Index Uint32:							" << features.fullDrawIndexUint32							<< std::endl;
			std::cout << "\tImage Cube Array:									" << features.imageCubeArray								<< std::endl;
			std::cout << "\tIndependent Blend:								" << features.independentBlend								<< std::endl;
			std::cout << "\tGeometry Shader:									" << features.geometryShader								<< std::endl;
			std::cout << "\tTesselation Shader:								" << features.tessellationShader							<< std::endl;
			std::cout << "\tSample Rate Shading:								" << features.sampleRateShading								<< std::endl;
			std::cout << "\tDual Src Blend:									" << features.dualSrcBlend									<< std::endl;
			std::cout << "\tLogic Op:											" << features.logicOp										<< std::endl;
			std::cout << "\tMulti Draw Indirect:								" << features.multiDrawIndirect								<< std::endl;
			std::cout << "\tDraw Indirect First Instance:						" << features.drawIndirectFirstInstance						<< std::endl;
			std::cout << "\tDepth Clamp:										" << features.depthClamp									<< std::endl;
			std::cout << "\tDepth Bias Clamp:									" << features.depthBiasClamp								<< std::endl;
			std::cout << "\tFill Mode Non Solid:								" << features.fillModeNonSolid								<< std::endl;
			std::cout << "\tDepth Bounds:										" << features.depthBounds									<< std::endl;
			std::cout << "\tWide Lines:										" << features.wideLines										<< std::endl;
			std::cout << "\tLarge Points:										" << features.largePoints									<< std::endl;
			std::cout << "\tAlpha To One:										" << features.alphaToOne									<< std::endl;
			std::cout << "\tMulti Viewport:									" << features.multiViewport									<< std::endl;
			std::cout << "\tSampler Anisotropy:								" << features.samplerAnisotropy								<< std::endl;
			std::cout << "\tTexture Compression ETC2:							" << features.textureCompressionETC2						<< std::endl;
			std::cout << "\tTexture Compression ASTC_LDR:						" << features.textureCompressionASTC_LDR					<< std::endl;
			std::cout << "\tTexture Compression BC:							" << features.textureCompressionBC							<< std::endl;
			std::cout << "\tOcclusion Query Precise:							" << features.occlusionQueryPrecise							<< std::endl;
			std::cout << "\tPipeline Statistics Query:						" << features.pipelineStatisticsQuery						<< std::endl;
			std::cout << "\tVertex Pipeline Stores and Atomics:				" << features.vertexPipelineStoresAndAtomics				<< std::endl;
			std::cout << "\tFragment Stores and Atomics:						" << features.fragmentStoresAndAtomics						<< std::endl;
			std::cout << "\tShader Tessellation and Geometry Point Size:		" << features.shaderTessellationAndGeometryPointSize		<< std::endl;
			std::cout << "\tShader Image Gather Extended:						" << features.shaderImageGatherExtended						<< std::endl;
			std::cout << "\tShader Storage Image Extended Formats:			" << features.shaderStorageImageExtendedFormats				<< std::endl;
			std::cout << "\tShader Storage Image Multisample:					" << features.shaderStorageImageMultisample					<< std::endl;
			std::cout << "\tShader Storage Image Read Without Format:			" << features.shaderStorageImageReadWithoutFormat			<< std::endl;
			std::cout << "\tShader Storage Image Write Without Format:		" << features.shaderStorageImageWriteWithoutFormat			<< std::endl;
			std::cout << "\tShader Uniform Buffer Array Dynamic Indexing:		" << features.shaderUniformBufferArrayDynamicIndexing		<< std::endl;
			std::cout << "\tShader Sampled Image Array Dynamic Indexing:		" << features.shaderSampledImageArrayDynamicIndexing		<< std::endl;
			std::cout << "\tShader Storage Buffer Array Dynamic Indexing:		" << features.shaderStorageBufferArrayDynamicIndexing		<< std::endl;
			std::cout << "\tShader Storage Image Array Dynamic Indexing:		" << features.shaderStorageImageArrayDynamicIndexing		<< std::endl;
			std::cout << "\tShader Clip Distance:								" << features.shaderClipDistance							<< std::endl;
			std::cout << "\tShader Cull Distance:								" << features.shaderCullDistance							<< std::endl;
			std::cout << "\tShader Float64:									" << features.shaderFloat64									<< std::endl;
			std::cout << "\tShader Int64:										" << features.shaderInt64									<< std::endl;
			std::cout << "\tShader Int16:										" << features.shaderInt16									<< std::endl;
			std::cout << "\tShader Resource Residency:						" << features.shaderResourceResidency						<< std::endl;
			std::cout << "\tShader Resource Min Load:							" << features.shaderResourceMinLod							<< std::endl;
			std::cout << "\tSparse Binding:									" << features.sparseBinding									<< std::endl;
			std::cout << "\tSparse Residency Buffer:							" << features.sparseResidencyBuffer							<< std::endl;
			std::cout << "\tSparse Residency Image2D:							" << features.sparseResidencyImage2D						<< std::endl;
			std::cout << "\tSparse Residency Image3D:							" << features.sparseResidencyImage3D						<< std::endl;
			std::cout << "\tSparse Residency 2 Samples:						" << features.sparseResidency2Samples						<< std::endl;
			std::cout << "\tSparse Residency 4 Samples:						" << features.sparseResidency4Samples						<< std::endl;
			std::cout << "\tSparse Residency 8 Samples:						" << features.sparseResidency8Samples						<< std::endl;
			std::cout << "\tSparse Residency 16 Samples:						" << features.sparseResidency16Samples						<< std::endl;
			std::cout << "\tSparse Residency Aliased:							" << features.sparseResidencyAliased						<< std::endl;
			std::cout << "\tVariable Multisample Rate:						" << features.variableMultisampleRate						<< std::endl;
			std::cout << "\tInherit Queries:									" << features.inheritedQueries								<< std::endl;
			std::cout << "--------------------------------------------------"															<< std::endl;

		}

		/*
		*	Function:		void vulkan::deviceMemoryProperties(VkPhysicalDevice &device)
		*	Purpose:		Prints the memory properties of the GPU
		*
		*/
		void deviceMemoryProperties(VkPhysicalDevice &device) {

			// Device memory properties
			VkPhysicalDeviceMemoryProperties memProp;
			vkGetPhysicalDeviceMemoryProperties(device, &memProp);

			std::cout << "Memory Type Count:	" << memProp.memoryTypeCount << std::endl;
			std::cout << "Memory HEAP Count:	" << memProp.memoryHeapCount << std::endl;

		}

		/*
		*	Function:		void vulkan::qeueFamilyProperties(VkPhysicalDevice &device)
		*	Purpose:		Prints the queue family properties
		*
		*/
		void queueFamilyProperties(VkPhysicalDevice &device) {

			// Queue family properties
			uint32_t amountOfQueueFamilies = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(

				device,							// Pass the device / GPU
				&amountOfQueueFamilies,			// Pass the variable to store amount of queue families
				NULL							// Pass no array to store them, so we can just get the number of queue families

			);
			VkQueueFamilyProperties* familyProperties = new VkQueueFamilyProperties[amountOfQueueFamilies];
			vkGetPhysicalDeviceQueueFamilyProperties(

				device,							// Pass the device / GPU
				&amountOfQueueFamilies,			// Pass the variable to store amount of queue families
				familyProperties				// Pass the array to store the enumerated queue families
			);

			std::cout << "Amount of Queue Families:	" << amountOfQueueFamilies << std::endl;

			for (unsigned int i = 0; i < amountOfQueueFamilies; i++) {

				std::cout << "Queue Family Number:				" << i																			<< std::endl;
				std::cout << "\tVK_QUEUE_GRAPHICS_BIT:			" << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)			<< std::endl;
				std::cout << "\tVK_QUEUE_COMPUTE_BIT:			" << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)				<< std::endl;
				std::cout << "\tVK_QUEUE_TRANSFER_BIT:			" << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)			<< std::endl;
				std::cout << "\tVK_QUEUE_SPARSE_BINDING_BIT:	" << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0)		<< std::endl;
				std::cout << "\tQueue Count:					" << familyProperties[i].queueCount												<< std::endl;
				std::cout << "\tTimestamp Valid Bits:			" << familyProperties[i].timestampValidBits										<< std::endl;

				uint32_t width		= familyProperties[i].minImageTransferGranularity.width;
				uint32_t height		= familyProperties[i].minImageTransferGranularity.height;
				uint32_t depth		= familyProperties[i].minImageTransferGranularity.depth;
				std::cout << "Min Image Timestamp Granularity:	" << width << "," << height << "," << depth									<< std::endl;

			}

			delete[] familyProperties;

		}

		/*
		*	Function:		void vulkan::deviceQueueCreateInfo(VkPhysicalDevice &device)
		*	Purpose:		Gather information to create logical device
		*
		*/
		// Device queue create info
		VkDeviceQueueCreateInfo deviceQueueCreateInfo;
		float queuePriorities[] = { 
			
			1.0f,
			1.0f,
			1.0f,
			1.0f
		
		};
		void deviceQueueCreateInfos(VkPhysicalDevice &device) {

			deviceQueueCreateInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.pNext					= NULL;
			deviceQueueCreateInfo.flags					= 0;
			deviceQueueCreateInfo.queueFamilyIndex		= 0;		// TODO: Enumerate best queue family and choose the correct index
			deviceQueueCreateInfo.queueCount			= 1;		// TODO: Check if amount is valid
			deviceQueueCreateInfo.pQueuePriorities		= queuePriorities;

		}

		/*
		*	Function:		void vulkan::deviceCreateInfo(VkPhysicalDevice &device)
		*	Purpose:		Prints the device creation info
		*
		*/
		
		// Device create info
		VkDeviceCreateInfo createInfo;
		void deviceCreateInfo() {

			// Physical device features
			VkPhysicalDeviceFeatures usedFeatures = {



			};

			const std::vector< const char* > deviceExtensions = {
			
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			
			};

			createInfo.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pNext						= NULL;
			createInfo.flags						= 0;
			createInfo.queueCreateInfoCount			= 1;
			createInfo.pQueueCreateInfos			= &deviceQueueCreateInfo;
			createInfo.enabledLayerCount			= 0;
			createInfo.ppEnabledLayerNames			= NULL;
			createInfo.enabledExtensionCount		= deviceExtensions.size();
			createInfo.ppEnabledExtensionNames		= deviceExtensions.data();
			createInfo.pEnabledFeatures				= &usedFeatures;
			
			device();
			
		}

		/*
		*	Function:		void vulkan::device()
		*	Purpose:		Creates the logical device from the physical device
		*
		*/
		void device() {

			// TODO: Pick best device instead of first device
			result = vkCreateDevice(
				
				physicalDevices[0], 
				&createInfo, 
				NULL, 
				&logicalDevice
			
			);
			ASSERT_VULKAN(result);

			logger.log(EVENT_LOG, "Device created successfully");

			createQueue();

		}

		/*
		*	Function:		void vulkan::createQueue()
		*	Purpose:		Creates the queue for the logical device
		*
		*/
		void createQueue() {

			vkGetDeviceQueue(
				
				logicalDevice, 
				0, 
				0, 
				&queue
			
			);

			VkBool32 surfaceSupport = false;

			result = vkGetPhysicalDeviceSurfaceSupportKHR(
				
				physicalDevices[0], 
				0, 
				surface,
				&surfaceSupport
			
			);
			ASSERT_VULKAN(result);

			if (!surfaceSupport) {
			
				logger.log(ERROR_LOG, "Surface not supported!");
				__debugbreak();

			}

		}

		
		/*
		*	Function:		void vulkan::createSurface()
		*	Purpose:		Would be used to create surface if we didn't do it with GLFW
		*
		*/
		// Surface create info
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
		void createSurface() {

			surfaceCreateInfo.sType			= VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
			surfaceCreateInfo.pNext			= NULL;
			surfaceCreateInfo.flags			= 0;
			surfaceCreateInfo.hinstance		= NULL;
			surfaceCreateInfo.hwnd			= NULL;

			result = vkCreateWin32SurfaceKHR(

				instance,
				&surfaceCreateInfo,
				NULL,
				&surface

			);
			ASSERT_VULKAN(result);

			logger.log(EVENT_LOG, "Surface created successfully");

		}

		/*
		*	Function:		void vulkan::surfaceCapabilities()
		*	Purpose:		Get stats of surface
		*
		*/
		// Surface Capabilities
		VkSurfaceCapabilitiesKHR surfaceCapabs;
		void surfaceCapabilities(VkPhysicalDevice &device) {
		
			result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			
				device,
				surface,
				&surfaceCapabs
			
			);
			ASSERT_VULKAN(result);

			std::cout << "Surface capabilities:	"														<< std::endl;
			std::cout << "\tminImageCount: "			<< surfaceCapabs.minImageCount					<< std::endl;
			std::cout << "\tmaxImageCount: "			<< surfaceCapabs.maxImageCount					<< std::endl;
			std::cout << "\tcurrentExtent: "			<< surfaceCapabs.currentExtent.width			<< " / " 
														<< surfaceCapabs.currentExtent.height			<< std::endl;
			std::cout << "\tminImageExtent: "			<< surfaceCapabs.minImageExtent.width			<< " / " 
														<< surfaceCapabs.minImageExtent.height			<< std::endl;
			std::cout << "\tmaxImageExtent: "			<< surfaceCapabs.maxImageExtent.width			<< " / " 
														<< surfaceCapabs.maxImageExtent.height			<< std::endl;
			std::cout << "\tmaxImageArrayLayers: "		<< surfaceCapabs.maxImageArrayLayers			<< std::endl;
			std::cout << "\tsupportedTransforms: "		<< surfaceCapabs.supportedTransforms			<< std::endl;
			std::cout << "\tcurrentTransform: "			<< surfaceCapabs.currentTransform				<< std::endl;
			std::cout << "\tsupportedCompositeAlpha: "	<< surfaceCapabs.supportedCompositeAlpha		<< std::endl;
			std::cout << "\tsupportedUsageFlags: "		<< surfaceCapabs.supportedUsageFlags			<< std::endl;
		
			uint32_t amountOfFormats = 0;
			result = vkGetPhysicalDeviceSurfaceFormatsKHR(
				
				device,
				surface,
				&amountOfFormats,
				nullptr
			
			);
			ASSERT_VULKAN(result);

			VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[amountOfFormats];
			result = vkGetPhysicalDeviceSurfaceFormatsKHR(
			
				device,
				surface,
				&amountOfFormats,
				surfaceFormats
						
			);
			ASSERT_VULKAN(result);

			std::cout << "-------------------------------" << std::endl;
			std::cout << "Amount of Formats:	" << amountOfFormats << std::endl;
			for (unsigned int i = 0; i < amountOfFormats; i++) {
			
				std::cout << "Format:	" << surfaceFormats[i].format << std::endl;
			
			}

			uint32_t amountOfPresentationModes = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(
			
				device,
				surface,
				&amountOfPresentationModes,
				nullptr

			);

			VkPresentModeKHR* presentModes = new VkPresentModeKHR[amountOfPresentationModes];
			vkGetPhysicalDeviceSurfacePresentModesKHR(

				device,
				surface,
				&amountOfPresentationModes,
				presentModes

			);

			std::cout << "Amount of Presentation Modes:	" << amountOfPresentationModes << std::endl;
			for (unsigned int i = 0; i < amountOfPresentationModes; i++) {
			
				std::cout << "\tSupported Presentation Mode: " << presentModes[i] << std::endl;
			
			}

			delete[] surfaceFormats;
			delete[] presentModes;

		}

		/*
		*	Function:		void vulkan::swapchain()
		*	Purpose:		Creates the swapchain
		*
		*/
		// SwapchainCreateInfo
		VkSwapchainCreateInfoKHR swapchainCreateInfo;
		void swapchainCreate() {
		
			swapchainCreateInfo.sType						= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainCreateInfo.pNext						= nullptr;
			swapchainCreateInfo.flags						= 0;
			swapchainCreateInfo.surface						= surface;
			swapchainCreateInfo.minImageCount				= 3;									// TODO: Check if valid
			swapchainCreateInfo.imageFormat					= colorAttachmentFormat;				// TODO: Check if valid
			swapchainCreateInfo.imageColorSpace				= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;	// TODO: Check if valid
			swapchainCreateInfo.imageExtent					= VkExtent2D {

																WINDOW_WIDTH, 
																WINDOW_HEIGHT

															};
			swapchainCreateInfo.imageArrayLayers			= 1;
			swapchainCreateInfo.imageUsage					= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapchainCreateInfo.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;			// TODO: Check if valid
			swapchainCreateInfo.queueFamilyIndexCount		= 0;
			swapchainCreateInfo.pQueueFamilyIndices			= nullptr;
			swapchainCreateInfo.preTransform				= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			swapchainCreateInfo.compositeAlpha				= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchainCreateInfo.presentMode					= VK_PRESENT_MODE_FIFO_KHR;				// TODO: Check if valid, VK_PRESENT_MODE_MAILBOX_KHR?
			swapchainCreateInfo.clipped						= VK_TRUE;
			swapchainCreateInfo.oldSwapchain				= VK_NULL_HANDLE;
		
			result = vkCreateSwapchainKHR(
			
				logicalDevice,
				&swapchainCreateInfo,
				nullptr,
				&swapchain
			
			);
			ASSERT_VULKAN(result);

			vkGetSwapchainImagesKHR(
			
				logicalDevice,
				swapchain,
				&amountOfImagesInSwapchain,
				nullptr
			
			);
			VkImage* swapchainImages = new VkImage[amountOfImagesInSwapchain];
			result = vkGetSwapchainImagesKHR(
			
				logicalDevice,
				swapchain,
				&amountOfImagesInSwapchain,
				swapchainImages
			
			);
			ASSERT_VULKAN(result);

			imageViews = new VkImageView[amountOfImagesInSwapchain];
			for (unsigned int i = 0; i < amountOfImagesInSwapchain; i++) {

				// Image view create info
				VkImageViewCreateInfo imageViewCreateInfo;

				imageViewCreateInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewCreateInfo.pNext								= nullptr;
				imageViewCreateInfo.flags								= 0;
				imageViewCreateInfo.image								= swapchainImages[i];
				imageViewCreateInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
				imageViewCreateInfo.format								= colorAttachmentFormat;			// TODO: Check if valid
				imageViewCreateInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.subresourceRange.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
				imageViewCreateInfo.subresourceRange.baseMipLevel		= 0;
				imageViewCreateInfo.subresourceRange.levelCount			= 1;
				imageViewCreateInfo.subresourceRange.baseArrayLayer		= 0;
				imageViewCreateInfo.subresourceRange.layerCount			= 1;
			
				result = vkCreateImageView(
				
					logicalDevice,
					&imageViewCreateInfo,
					nullptr,
					&imageViews[i]
				
				);
				ASSERT_VULKAN(result);

			}

			std::vector< char > shaderCodeVert = readFile("vert.spv");
			std::vector< char > shaderCodeFrag = readFile("frag.spv");

			createShaderModule(shaderCodeVert, &shaderModuleVert);
			createShaderModule(shaderCodeFrag, &shaderModuleFrag);

			VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert;
			shaderStageCreateInfoVert.sType						= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCreateInfoVert.pNext						= nullptr;
			shaderStageCreateInfoVert.flags						= 0;
			shaderStageCreateInfoVert.stage						= VK_SHADER_STAGE_VERTEX_BIT;
			shaderStageCreateInfoVert.module					= shaderModuleVert;
			shaderStageCreateInfoVert.pName						= "main";
			shaderStageCreateInfoVert.pSpecializationInfo		= nullptr;

			VkPipelineShaderStageCreateInfo shaderStageCreateInfoFrag;
			shaderStageCreateInfoFrag.sType						= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCreateInfoFrag.pNext						= nullptr;
			shaderStageCreateInfoFrag.flags						= 0;
			shaderStageCreateInfoFrag.stage						= VK_SHADER_STAGE_FRAGMENT_BIT;
			shaderStageCreateInfoFrag.module					= shaderModuleFrag;
			shaderStageCreateInfoFrag.pName						= "main";
			shaderStageCreateInfoFrag.pSpecializationInfo		= nullptr;

			VkPipelineShaderStageCreateInfo shaderStages[] = {
			
				shaderStageCreateInfoVert,
				shaderStageCreateInfoFrag
			
			};

			VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo;
			vertexInputCreateInfo.sType								= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputCreateInfo.pNext								= nullptr;
			vertexInputCreateInfo.flags								= 0;
			vertexInputCreateInfo.vertexBindingDescriptionCount		= 0;
			vertexInputCreateInfo.pVertexBindingDescriptions		= nullptr;
			vertexInputCreateInfo.vertexAttributeDescriptionCount	= 0;
			vertexInputCreateInfo.pVertexAttributeDescriptions		= nullptr;

			VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
			inputAssemblyCreateInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyCreateInfo.pNext						= nullptr;
			inputAssemblyCreateInfo.flags						= 0;
			inputAssemblyCreateInfo.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssemblyCreateInfo.primitiveRestartEnable		= VK_FALSE;

			viewport.x				= 0.0f;
			viewport.y				= 0.0f;
			viewport.width			= WINDOW_WIDTH;
			viewport.height			= WINDOW_HEIGHT;
			viewport.minDepth		= 0.0f;
			viewport.maxDepth		= 1.0f;

			VkRect2D scissor;
			scissor.offset = {
			
				0,
				0

			};
			scissor.extent = {
			
				WINDOW_WIDTH,
				WINDOW_HEIGHT
			
			};

			VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
			viewportStateCreateInfo.sType				= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportStateCreateInfo.pNext				= nullptr;
			viewportStateCreateInfo.flags				= 0;
			viewportStateCreateInfo.viewportCount		= 1;
			viewportStateCreateInfo.pViewports			= &viewport;
			viewportStateCreateInfo.scissorCount		= 1;
			viewportStateCreateInfo.pScissors			= &scissor;

			VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo;
			rasterizationCreateInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationCreateInfo.pNext						= nullptr;
			rasterizationCreateInfo.flags						= 0;
			rasterizationCreateInfo.depthClampEnable			= VK_FALSE;
			rasterizationCreateInfo.rasterizerDiscardEnable		= VK_TRUE;
			rasterizationCreateInfo.polygonMode					= VK_POLYGON_MODE_FILL;
			rasterizationCreateInfo.cullMode					= VK_CULL_MODE_BACK_BIT;
			rasterizationCreateInfo.frontFace					= VK_FRONT_FACE_CLOCKWISE;
			rasterizationCreateInfo.depthBiasEnable				= VK_FALSE;
			rasterizationCreateInfo.depthBiasConstantFactor		= 0.0f;
			rasterizationCreateInfo.depthBiasClamp				= 0.0f;
			rasterizationCreateInfo.depthBiasSlopeFactor		= 0.0f;
			rasterizationCreateInfo.lineWidth					= 1.0f;

			VkPipelineMultisampleStateCreateInfo multisampleCreateInfo;
			multisampleCreateInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleCreateInfo.pNext						= nullptr;
			multisampleCreateInfo.flags						= 0;
			multisampleCreateInfo.rasterizationSamples		= VK_SAMPLE_COUNT_1_BIT;
			multisampleCreateInfo.sampleShadingEnable		= VK_FALSE;
			multisampleCreateInfo.minSampleShading			= 1.0f;
			multisampleCreateInfo.pSampleMask				= nullptr;
			multisampleCreateInfo.alphaToCoverageEnable		= VK_FALSE;
			multisampleCreateInfo.alphaToOneEnable			= VK_FALSE;

			VkPipelineColorBlendAttachmentState colorBlendAttachment;
			colorBlendAttachment.blendEnable				= VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor		= VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor		= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp				= VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor		= VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor		= VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp				= VK_BLEND_OP_ADD;
			colorBlendAttachment.colorWriteMask				= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

			VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo;
			colorBlendCreateInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendCreateInfo.pNext					= nullptr;
			colorBlendCreateInfo.flags					= 0;
			colorBlendCreateInfo.logicOpEnable			= VK_FALSE;
			colorBlendCreateInfo.logicOp				= VK_LOGIC_OP_NO_OP;
			colorBlendCreateInfo.attachmentCount		= 1;
			colorBlendCreateInfo.pAttachments			= &colorBlendAttachment;
			colorBlendCreateInfo.blendConstants[0]		= 0.0f;
			colorBlendCreateInfo.blendConstants[1]		= 0.0f;
			colorBlendCreateInfo.blendConstants[2]		= 0.0f;
			colorBlendCreateInfo.blendConstants[3]		= 0.0f;

			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
			pipelineLayoutCreateInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.pNext						= nullptr;
			pipelineLayoutCreateInfo.flags						= 0;
			pipelineLayoutCreateInfo.setLayoutCount				= 0;
			pipelineLayoutCreateInfo.pSetLayouts				= nullptr;
			pipelineLayoutCreateInfo.pushConstantRangeCount		= 0;
			pipelineLayoutCreateInfo.pPushConstantRanges		= nullptr;

			result = vkCreatePipelineLayout(
			
				logicalDevice,
				&pipelineLayoutCreateInfo,
				nullptr,
				&pipelineLayout
			
			);
			ASSERT_VULKAN(result);

			VkAttachmentDescription attachmentDescription;
			attachmentDescription.flags				= 0;
			attachmentDescription.format			= colorAttachmentFormat;
			attachmentDescription.samples			= VK_SAMPLE_COUNT_1_BIT;
			attachmentDescription.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachmentDescription.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDescription.stencilLoadOp		= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescription.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentDescription.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDescription.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference attachmentReference;
			attachmentReference.attachment		= 0;
			attachmentReference.layout			= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpassDescription;
			subpassDescription.flags						= 0;
			subpassDescription.pipelineBindPoint			= VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.inputAttachmentCount			= 0;
			subpassDescription.pInputAttachments			= nullptr;
			subpassDescription.colorAttachmentCount			= 1;
			subpassDescription.pColorAttachments			= &attachmentReference;
			subpassDescription.pResolveAttachments			= nullptr;
			subpassDescription.pDepthStencilAttachment		= nullptr;
			subpassDescription.preserveAttachmentCount		= 0;
			subpassDescription.pPreserveAttachments			= nullptr;

			VkSubpassDependency subpassDependency;
			subpassDependency.srcSubpass			= VK_SUBPASS_EXTERNAL;
			subpassDependency.dstSubpass			= 0;
			subpassDependency.srcStageMask			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.dstStageMask			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependency.srcAccessMask			= 0;
			subpassDependency.dstAccessMask			= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dependencyFlags		= 0;

			VkRenderPassCreateInfo renderPassCreateInfo;
			renderPassCreateInfo.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.pNext					= nullptr;
			renderPassCreateInfo.flags					= 0;
			renderPassCreateInfo.attachmentCount 		= 1;
			renderPassCreateInfo.pAttachments			= &attachmentDescription;
			renderPassCreateInfo.subpassCount			= 1;
			renderPassCreateInfo.pSubpasses				= &subpassDescription;
			renderPassCreateInfo.dependencyCount		= 0;
			renderPassCreateInfo.pDependencies			= &subpassDependency;

			result = vkCreateRenderPass(
			
				logicalDevice,
				&renderPassCreateInfo,
				nullptr,
				&renderPass
			
			);
			ASSERT_VULKAN(result);

			VkGraphicsPipelineCreateInfo pipelineCreateInfo;
			pipelineCreateInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineCreateInfo.pNext					= nullptr;
			pipelineCreateInfo.flags					= 0;
			pipelineCreateInfo.stageCount				= 2;
			pipelineCreateInfo.pStages					= shaderStages;
			pipelineCreateInfo.pVertexInputState		= &vertexInputCreateInfo;
			pipelineCreateInfo.pInputAssemblyState		= &inputAssemblyCreateInfo;
			pipelineCreateInfo.pTessellationState		= nullptr;
			pipelineCreateInfo.pViewportState			= &viewportStateCreateInfo;
			pipelineCreateInfo.pRasterizationState		= &rasterizationCreateInfo;
			pipelineCreateInfo.pMultisampleState		= &multisampleCreateInfo;
			pipelineCreateInfo.pDepthStencilState		= nullptr;
			pipelineCreateInfo.pColorBlendState			= &colorBlendCreateInfo;
			pipelineCreateInfo.pDynamicState			= nullptr;
			pipelineCreateInfo.layout					= pipelineLayout;
			pipelineCreateInfo.renderPass				= renderPass;
			pipelineCreateInfo.subpass					= 0;
			pipelineCreateInfo.basePipelineHandle		= VK_NULL_HANDLE;
			pipelineCreateInfo.basePipelineIndex		= -1;

			result = vkCreateGraphicsPipelines(
			
				logicalDevice,
				VK_NULL_HANDLE,
				1,
				&pipelineCreateInfo,
				nullptr,
				&pipeline

			);
			ASSERT_VULKAN(result);

			framebuffers = new VkFramebuffer[amountOfImagesInSwapchain];
			for (size_t i = 0; i < amountOfImagesInSwapchain; i++) {
			
				VkFramebufferCreateInfo frambufferCreateInfo;
				frambufferCreateInfo.sType				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				frambufferCreateInfo.pNext				= nullptr;
				frambufferCreateInfo.flags				= 0;
				frambufferCreateInfo.renderPass			= renderPass;
				frambufferCreateInfo.attachmentCount	= 1;
				frambufferCreateInfo.pAttachments		= &(imageViews[i]);
				frambufferCreateInfo.width				= WINDOW_WIDTH;
				frambufferCreateInfo.height				= WINDOW_HEIGHT;
				frambufferCreateInfo.layers				= 1;

				result = vkCreateFramebuffer(
					
					logicalDevice, 
					&frambufferCreateInfo,
					nullptr, 
					&(framebuffers[i])
				
				);
				ASSERT_VULKAN(result);

			}

			VkCommandPoolCreateInfo commandPoolCreateInfo;
			commandPoolCreateInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext					= nullptr;
			commandPoolCreateInfo.flags					= 0;
			commandPoolCreateInfo.queueFamilyIndex		= 0;		// TODO: Check if valid

			result = vkCreateCommandPool(
			
				logicalDevice,
				&commandPoolCreateInfo,
				nullptr,
				&commandPool
			
			);
			ASSERT_VULKAN(result);

			VkCommandBufferAllocateInfo commandBufferAllocateInfo;
			commandBufferAllocateInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.pNext					= nullptr;
			commandBufferAllocateInfo.commandPool			= commandPool;
			commandBufferAllocateInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount	= amountOfImagesInSwapchain;

			commandBuffers = new VkCommandBuffer[amountOfImagesInSwapchain];
			result = vkAllocateCommandBuffers(
				
				logicalDevice,
				&commandBufferAllocateInfo,
				commandBuffers
			
			);
			ASSERT_VULKAN(result);

			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext				= nullptr;
			commandBufferBeginInfo.flags				= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			commandBufferBeginInfo.pInheritanceInfo		= nullptr;

			for (size_t i = 0; i < amountOfImagesInSwapchain; i++) {
			
				result = vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo);
				ASSERT_VULKAN(result);

				VkRenderPassBeginInfo renderPassBeginInfo;
				renderPassBeginInfo.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassBeginInfo.pNext					= nullptr;
				renderPassBeginInfo.renderPass				= renderPass;
				renderPassBeginInfo.framebuffer				= framebuffers[i];
				renderPassBeginInfo.renderArea.offset		= { 0, 0 };
				renderPassBeginInfo.renderArea.extent		= { WINDOW_WIDTH, WINDOW_HEIGHT };
				VkClearValue clearValue						= { 0.0f, 0.0f, 0.0f, 1.0f };
				renderPassBeginInfo.clearValueCount			= 1;
				renderPassBeginInfo.pClearValues			= &clearValue;

				vkCmdBeginRenderPass(
				
					commandBuffers[i], 
					&renderPassBeginInfo,
					VK_SUBPASS_CONTENTS_INLINE
				
				);

				vkCmdBindPipeline(
					
					commandBuffers[i],
					VK_PIPELINE_BIND_POINT_GRAPHICS, 
					pipeline
				
				);

				vkCmdDraw(
					
					commandBuffers[i], 
					3, 
					1,
					0, 
					0
				
				);

				vkCmdEndRenderPass(commandBuffers[i]);

				result = vkEndCommandBuffer(commandBuffers[i]);
				ASSERT_VULKAN(result);
			
			}

			VkSemaphoreCreateInfo semaphoreCreateInfo;
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphoreCreateInfo.pNext = nullptr;
			semaphoreCreateInfo.flags = 0;

			result = vkCreateSemaphore(
				
				logicalDevice, 
				&semaphoreCreateInfo, 
				nullptr, 
				&semaphoreImageAvailable
			
			);
			ASSERT_VULKAN(result);
			result = vkCreateSemaphore(
				
				logicalDevice, 
				&semaphoreCreateInfo,
				nullptr, 
				&semaphoreRenderingFinished
			
			);
			ASSERT_VULKAN(result);


			delete[] swapchainImages;
			delete[] layers;
			delete[] extensions;

		}

		/*
		*	Function:		void vulkan::createShaderModule(const std::vector< char >& code, VkShaderModule* shaderModule)
		*	Purpose:		Creates a shader module
		*
		*/
		void createShaderModule(const std::vector< char >& code, VkShaderModule* shaderModule) {

			VkShaderModuleCreateInfo shaderCreateInfo;
			shaderCreateInfo.sType			= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderCreateInfo.pNext			= nullptr;
			shaderCreateInfo.flags			= 0;
			shaderCreateInfo.codeSize		= code.size();
			shaderCreateInfo.pCode			= (uint32_t*)code.data();

			result = vkCreateShaderModule(
			
				logicalDevice,
				&shaderCreateInfo,
				nullptr,
				shaderModule
			
			);
			ASSERT_VULKAN(result);
		
		}

		/*
		*	Function:		std::vector< char > vulkan::readFile(const std::string &filename)
		*	Purpose:		Reads files and loads them to HEAP
		*
		*/
		std::vector< char > readFile(const std::string &filename) {
		
			std::ifstream file(filename, std::ios::binary | std::ios::ate);
		
			if (file) {
			
				size_t filesize = static_cast< size_t >(file.tellg());
				std::vector< char > fileBuffer(filesize);
				file.seekg(0);
				file.read(fileBuffer.data(), filesize);
				file.close();
				return fileBuffer;
			
			}
			else {
			
				logger.log(ERROR_LOG, "Failed to open shader-file at " + filename);
				throw std::runtime_error("Failed to open shader-file at " + filename);
			
			}

		}

		/*
		*	Function:		void vulkan::shutdownVulkan()
		*	Purpose:		Handles main shutdown of the VULKAN API
		*
		*/
		void shutdownVulkan() {

			vulkan::logger.log(START_STOP_LOG, "Shutdown initialized...");

			result = vkDeviceWaitIdle(logicalDevice);
			ASSERT_VULKAN(result);

			vkDestroySemaphore(logicalDevice, semaphoreImageAvailable, nullptr);
			vkDestroySemaphore(logicalDevice, semaphoreRenderingFinished, nullptr);

			vkFreeCommandBuffers(
				
				logicalDevice,
				commandPool, 
				amountOfImagesInSwapchain,
				commandBuffers

			);
			delete[] commandBuffers;

			vkDestroyCommandPool(
				
				logicalDevice,
				commandPool, 
				nullptr);

			for (size_t i = 0; i < amountOfImagesInSwapchain; i++) {
			
				vkDestroyFramebuffer(
				
					logicalDevice,
					framebuffers[i],
					nullptr
				
				);
			
			}
			delete[] framebuffers;

			vkDestroyPipeline(

				logicalDevice,
				pipeline,
				nullptr

			);

			vkDestroyRenderPass(
			
				logicalDevice,
				renderPass,
				nullptr
			
			);

			for (unsigned int i = 0; i < amountOfImagesInSwapchain; i++) {
			
				vkDestroyImageView(
				
					logicalDevice,
					imageViews[i],
					nullptr
				
				);
			
			}
			delete[] imageViews;

			vkDestroyPipelineLayout(
			
				logicalDevice,
				pipelineLayout,
				nullptr
			
			);

			vkDestroyShaderModule(
				
				logicalDevice, 
				shaderModuleVert,
				nullptr
			
			); 

			vkDestroyShaderModule(

				logicalDevice,
				shaderModuleFrag,
				nullptr

			);

			vkDestroySwapchainKHR(
				
				logicalDevice, 
				swapchain, 
				nullptr
			
			);
			vkDestroyDevice(logicalDevice, NULL);
			vkDestroySurfaceKHR(

				instance,
				surface,
				NULL

			);
			vkDestroyInstance(instance, NULL);
			delete[] vulkan::physicalDevices;

		}

		/*
		*	Function:		vulkan::drawFrame()
		*	Purpose:		Renders a frame to the screen
		*
		*/
		void drawFrame() {
		
			uint32_t imageIndex;
			vkAcquireNextImageKHR(
			
				logicalDevice,
				swapchain,
				(size_t)(std::numeric_limits< uint64_t >::max),
				semaphoreImageAvailable,
				VK_NULL_HANDLE,
				&imageIndex
			
			);

			VkSubmitInfo submitInfo;
			submitInfo.sType						= VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext						= nullptr;
			submitInfo.waitSemaphoreCount			= 1;
			submitInfo.pWaitSemaphores				= &semaphoreImageAvailable;
			VkPipelineStageFlags waitStageMask[]	= { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.pWaitDstStageMask			= waitStageMask;
			submitInfo.commandBufferCount			= 1;
			submitInfo.pCommandBuffers				= &(commandBuffers[imageIndex]);
			submitInfo.signalSemaphoreCount			= 1;
			submitInfo.pSignalSemaphores			= &semaphoreRenderingFinished;

			result = vkQueueSubmit(
			
				queue, 
				1,
				&submitInfo, 
				VK_NULL_HANDLE
			
			);
			ASSERT_VULKAN(result);

			VkPresentInfoKHR presentInfo;
			presentInfo.sType					= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pNext					= nullptr;
			presentInfo.waitSemaphoreCount		= 1;
			presentInfo.pWaitSemaphores			= &semaphoreRenderingFinished;
			presentInfo.swapchainCount			= 1;
			presentInfo.pSwapchains				= &swapchain;
			presentInfo.pImageIndices			= &imageIndex;
			presentInfo.pResults				= nullptr;

			result = vkQueuePresentKHR(
			
				queue,
				&presentInfo
			
			);
			ASSERT_VULKAN(result);

		}

	}

	/*
	*	Namespace:		glfw
	*	Purpose:		Differenciate GLFW from VULKAN
	*
	*/
	namespace glfw {

		/*
		*	Function:		void glfw::init()
		*	Purpose:		Handles GLFW-Initialization
		*
		*/
		void init() {

			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			window = glfwCreateWindow(
				
				WINDOW_WIDTH,
				WINDOW_HEIGHT, 
				TITLE, 
				nullptr, 
				nullptr
			
			);

		}

		/*
		*	Function:		void glfw::gameLoop()
		*	Purpose:		Contains the main game loop
		*
		*/
		void gameLoop() {

			while (!glfwWindowShouldClose(window)) {

				glfwPollEvents();
				vulkan::drawFrame();

			}

		}

		/*
		*	Function:		void glfw::shutdownGLFW()
		*	Purpose:		Handles main shutdown of GLFW
		*
		*/
		void shutdownGLFW() {

			glfwDestroyWindow(window);

		}

	}

}

/*
*	Function:		int main()
*	Purpose:		Entry point for the application
*
*/
int main() {

	game::glfw::init();
	game::vulkan::init();
	game::glfw::gameLoop();
	game::vulkan::shutdownVulkan();
	game::glfw::shutdownGLFW();

	return 0;

}