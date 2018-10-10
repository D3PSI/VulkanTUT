/*
*	Main.cpp: Defines the entry point for the console application.
*
*/
#include "vulkan/vulkan.h"
#include <iostream>
#include <iomanip>
#include <intrin.h>
#include <vector>

/*
*	Makro:			ASSERT_VULKAN(val)
*	Purpose:		Checks for error codes and pauses code execution if necessary
*	
*/
#define ASSERT_VULKAN(val)\
	if(val != VK_SUCCESS) {\
		__debugbreak();\
	}

/*
*	Prototypes
*
*/
namespace vulk {

	void init(void);
	void deviceProperties(VkPhysicalDevice &device);
	void deviceFeatures(VkPhysicalDevice &device);
	void deviceMemoryProperties(VkPhysicalDevice &device);
	void queueFamilyProperties(VkPhysicalDevice &device);
	void deviceQueueCreateInfos(VkPhysicalDevice &device);
	void deviceCreateInfo(VkPhysicalDevice &device);

}

/*
*	Global Variables
*
*/
VkResult result; 
VkPhysicalDevice *physicalDevices;
VkInstance instance;
VkDevice logicalDevice;

/*
*	Namespace:		vulk
*	Purpose:		Prevent stupid naming conflicts
*
*/
namespace vulk {

	/*
	*	Function:		void vulk::init()
	*	Purpose:		Initializes the Vulkan API
	*	
	*/
	void init() {

		// Application info
		VkApplicationInfo appInfo;
		appInfo.sType							= VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext							= NULL;
		appInfo.pApplicationName				= "VULKAN TUTORIAL";
		appInfo.applicationVersion				= VK_MAKE_VERSION(0, 0, 0);
		appInfo.pEngineName						= "Tutorial Vulkan Engine";
		appInfo.engineVersion					= VK_MAKE_VERSION(0, 0, 0);
		appInfo.apiVersion						= VK_API_VERSION_1_1;


		uint32_t amountOfLayers = 0;
		vkEnumerateInstanceLayerProperties(&amountOfLayers, NULL);
		VkLayerProperties *layers = new VkLayerProperties[amountOfLayers];
		vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);

		std::cout << "Amount of instance layers:	" << amountOfLayers << std::endl;
		for (unsigned int i = 0; i < amountOfLayers; i++) {
		
			std::cout << "Name:	"				<< layers[i].layerName					<< std::endl;
			std::cout << "Spec-Version:	"		<< layers[i].specVersion				<< std::endl;
			std::cout << "Impl-Version:	"		<< layers[i].implementationVersion		<< std::endl;
			std::cout << "Description:	"		<< layers[i].description				<< std::endl;
			std::cout << "------------------"	<< std::endl;

		}

		const std::vector<const char*> validationLayers = {
			
			"VK_LAYER_LUNARG_standard_validation"
		
		};

		// Instance info
		VkInstanceCreateInfo instanceInfo;
		instanceInfo.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pNext						= NULL;
		instanceInfo.flags						= 0;
		instanceInfo.pApplicationInfo			= &appInfo;
		instanceInfo.enabledLayerCount			= validationLayers.size();
		instanceInfo.ppEnabledLayerNames		= validationLayers.data();
		instanceInfo.enabledExtensionCount		= 0;
		instanceInfo.ppEnabledExtensionNames	= nullptr;

		// Instance creation
		result = vkCreateInstance(

			&instanceInfo,	// Pass instance info
			NULL,			// Pass no allocation callback
			&instance		// Pass the actual instance

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
			deviceCreateInfo(physicalDevices[i]);

		}

	}

	/*
	*	Function:		vulk::printDeviceProperties(vkPhysicalDevice &device)
	*	Purpose:		Prints the device information for every GPU
	*	
	*/
	void deviceProperties(VkPhysicalDevice &device) {
	
		// Device properties
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		uint32_t apiVer = properties.apiVersion;

		std::cout << "Name:	"										  << properties.deviceName								 << std::endl;
		std::cout << "API-Version:	" << VK_VERSION_MAJOR(apiVer) << 
			"." << VK_VERSION_MINOR(apiVer) << "." <<
			VK_VERSION_PATCH(apiVer)																						 << std::endl;
		std::cout << "Discrete-Queue-Priorities:	"				  << properties.limits.discreteQueuePriorities			 << std::endl;
		std::cout << "Driver-Version:	"							  << properties.driverVersion							 << std::endl;
		std::cout << "Vendor-ID:	"								  << properties.vendorID								 << std::endl;
		std::cout << "Device-ID:	"								  << properties.deviceID								 << std::endl;
		std::cout << "Device-Type:	"								  << properties.deviceType								 << std::endl;
		std::cout << "Pipeline-Cache-UUID:	"						  << properties.pipelineCacheUUID						 << std::endl;
		std::cout << "------------------"																					 << std::endl;

	}

	/*
	*	Function:		void printDeviceFeatures(VkPhysicalDevice &device)
	*	Purpose:		Prints detailed information about the GPU
	*	
	*/
	void deviceFeatures(VkPhysicalDevice &device) {
	
		// Device features
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(device, &features);

		std::cout << "Features:"																							 << std::endl;
		std::cout << "Robust Buffer Access:	"						  << features.robustBufferAccess						 << std::endl;
		std::cout << "Full Draw Index Uint32:	"					  << features.fullDrawIndexUint32						 << std::endl;
		std::cout << "Image Cube Array:	"							  << features.imageCubeArray							 << std::endl;
		std::cout << "Independent Blend:	"						  << features.independentBlend							 << std::endl;
		std::cout << "Geometry Shader:	"							  << features.geometryShader							 << std::endl;
		std::cout << "Tesselation Shader:	"						  << features.tessellationShader						 << std::endl;
		std::cout << "Sample Rate Shading:	"						  << features.sampleRateShading							 << std::endl;
		std::cout << "Dual Src Blend:	"							  << features.dualSrcBlend								 << std::endl;
		std::cout << "Logic Op:	"									  << features.logicOp									 << std::endl;
		std::cout << "Multi Draw Indirect:	"						  << features.multiDrawIndirect							 << std::endl;
		std::cout << "Draw Indirect First Instance:	"				  << features.drawIndirectFirstInstance					 << std::endl;
		std::cout << "Depth Clamp:	"								  << features.depthClamp								 << std::endl;
		std::cout << "Depth Bias Clamp:	"							  << features.depthBiasClamp							 << std::endl;
		std::cout << "Fill Mode Non Solid:	"						  << features.fillModeNonSolid							 << std::endl;
		std::cout << "Depth Bounds:	"								  << features.depthBounds								 << std::endl;
		std::cout << "Wide Lines:	"								  << features.wideLines									 << std::endl;
		std::cout << "Large Points:	"								  << features.largePoints								 << std::endl;
		std::cout << "Alpha To One:	"								  << features.alphaToOne								 << std::endl;
		std::cout << "Multi Viewport:	"							  << features.multiViewport							     << std::endl;
		std::cout << "Sampler Anisotropy:	"						  << features.samplerAnisotropy							 << std::endl;
		std::cout << "Texture Compression ETC2:	"					  << features.textureCompressionETC2					 << std::endl;
		std::cout << "Texture Compression ASTC_LDR:	"				  << features.textureCompressionASTC_LDR				 << std::endl;
		std::cout << "Texture Compression BC:	"					  << features.textureCompressionBC						 << std::endl;
		std::cout << "Occlusion Query Precise:	"					  << features.occlusionQueryPrecise					     << std::endl;
		std::cout << "Pipeline Statistics Query:	"				  << features.pipelineStatisticsQuery					 << std::endl;
		std::cout << "Vertex Pipeline Stores and Atomics:	"		  << features.vertexPipelineStoresAndAtomics			 << std::endl;
		std::cout << "Fragment Stores and Atomics:	"				  << features.fragmentStoresAndAtomics					 << std::endl;
		std::cout << "Shader Tessellation and Geometry Point Size:	" << features.shaderTessellationAndGeometryPointSize	 << std::endl;
		std::cout << "Shader Image Gather Extended:	"				  << features.shaderImageGatherExtended					 << std::endl;
		std::cout << "Shader Storage Image Extended Formats:	"	  << features.shaderStorageImageExtendedFormats			 << std::endl;
		std::cout << "Shader Storage Image Multisample:	"			  << features.shaderStorageImageMultisample				 << std::endl;
		std::cout << "Shader Storage Image Read Without Format:	"	  << features.shaderStorageImageReadWithoutFormat		 << std::endl;
		std::cout << "Shader Storage Image Write Without Format:	" << features.shaderStorageImageWriteWithoutFormat		 << std::endl;
		std::cout << "Shader Uniform Buffer Array Dynamic Indexing:	" << features.shaderUniformBufferArrayDynamicIndexing	 << std::endl;
		std::cout << "Shader Sampled Image Array Dynamic Indexing:	" << features.shaderSampledImageArrayDynamicIndexing	 << std::endl;
		std::cout << "Shader Storage Buffer Array Dynamic Indexing:	" << features.shaderStorageBufferArrayDynamicIndexing	 << std::endl;
		std::cout << "Shader Storage Image Array Dynamic Indexing:	" << features.shaderStorageImageArrayDynamicIndexing	 << std::endl;
		std::cout << "Shader Clip Distance:	"						  << features.shaderClipDistance						 << std::endl;
		std::cout << "Shader Cull Distance:	"						  << features.shaderCullDistance						 << std::endl;
		std::cout << "Shader Float64:	"							  << features.shaderFloat64								 << std::endl;
		std::cout << "Shader Int64:	"								  << features.shaderInt64								 << std::endl;
		std::cout << "Shader Int16:	"								  << features.shaderInt16								 << std::endl;
		std::cout << "Shader Resource Residency:	"				  << features.shaderResourceResidency					 << std::endl;
		std::cout << "Shader Resource Min Load:	"					  << features.shaderResourceMinLod						 << std::endl;
		std::cout << "Sparse Binding:	"							  << features.sparseBinding								 << std::endl;
		std::cout << "Sparse Residency Buffer:	"					  << features.sparseResidencyBuffer						 << std::endl;
		std::cout << "Sparse Residency Image2D:	"					  << features.sparseResidencyImage2D					 << std::endl;
		std::cout << "Sparse Residency Image3D:	"					  << features.sparseResidencyImage3D					 << std::endl;
		std::cout << "Sparse Residency 2 Samples:	"				  << features.sparseResidency2Samples					 << std::endl;
		std::cout << "Sparse Residency 4 Samples:	"				  << features.sparseResidency4Samples					 << std::endl;
		std::cout << "Sparse Residency 8 Samples:	"				  << features.sparseResidency8Samples					 << std::endl;
		std::cout << "Sparse Residency 16 Samples:	"				  << features.sparseResidency16Samples					 << std::endl;
		std::cout << "Sparse Residency Aliased:	"					  << features.sparseResidencyAliased					 << std::endl;
		std::cout << "Variable Multisample Rate:	"				  << features.variableMultisampleRate					 << std::endl;
		std::cout << "Inherit Queries:	"							  << features.inheritedQueries							 << std::endl;
		std::cout << "------------------"																					 << std::endl;
		
	}

	/*
	*	Function:		void printDeviceMemoryProperties(VkPhysicalDevice &device)
	*	Purpose:		Prints the memory properties of the GPU
	*	
	*/
	void deviceMemoryProperties(VkPhysicalDevice &device) {

		// Device memory properties
		VkPhysicalDeviceMemoryProperties memProp;
		vkGetPhysicalDeviceMemoryProperties(device, &memProp);

		std::cout << "Memory Type Count:	"						  << memProp.memoryTypeCount							 << std::endl;
		std::cout << "Memory HEAP Count:	"						  << memProp.memoryHeapCount							 << std::endl;
	}

	/*
	*	Function:		void printQueueFamilyProperties(VkPhysicalDevice &device)
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
		VkQueueFamilyProperties *familyProperties = new VkQueueFamilyProperties[amountOfQueueFamilies];
		vkGetPhysicalDeviceQueueFamilyProperties(

			device,							// Pass the device / GPU
			&amountOfQueueFamilies,			// Pass the variable to store amount of queue families
			familyProperties				// Pass the array to store the enumerated queue families
		);

		std::cout << "Amount of Queue Families:	"					  << amountOfQueueFamilies								 << std::endl;

		for (unsigned int i = 0; i < amountOfQueueFamilies; i++) {
		
			std::cout << "Queue Family Number:	"					  << i													 << std::endl;
			std::cout << "VK_QUEUE_GRAPHICS_BIT:	"				  << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)		<< std::endl;
			std::cout << "VK_QUEUE_COMPUTE_BIT:	"					  << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)			<< std::endl;
			std::cout << "VK_QUEUE_TRANSFER_BIT:	"				  << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)		<< std::endl;
			std::cout << "VK_QUEUE_SPARSE_BINDING_BIT:	"			  << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0)	<< std::endl;
			std::cout << "Queue Count:	"							  << familyProperties[i].queueCount											<< std::endl;
			std::cout << "Timestamp Valid Bits:	"					  << familyProperties[i].timestampValidBits									<< std::endl;
			
			uint32_t width = familyProperties[i].minImageTransferGranularity.width;
			uint32_t height = familyProperties[i].minImageTransferGranularity.height;
			uint32_t depth = familyProperties[i].minImageTransferGranularity.depth;
			std::cout << "Min Image Timestamp Granularity:	"		  << width << "," << height << "," << depth									<< std::endl;

		}
	
		delete[] familyProperties;

	}

	/*
	*	Function:		void printDeviceQueueCreateInfo(VkPhysicalDevice &device)
	*	Purpose:		Gather information to create logical device
	*	
	*/
	// Device queue create info
	VkDeviceQueueCreateInfo deviceQueueCreateInfo;
	float queuePriorities[] = {1.0f, 1.0f, 1.0f, 1.0f};
	void deviceQueueCreateInfos(VkPhysicalDevice &device) {
	
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceQueueCreateInfo.pNext = NULL;
		deviceQueueCreateInfo.flags = 0;
		deviceQueueCreateInfo.queueFamilyIndex = 0;		// TODO: Enumerate best queue family and choose the correct index
		deviceQueueCreateInfo.queueCount = 4;			// TODO: Check if amount is valid
		deviceQueueCreateInfo.pQueuePriorities = queuePriorities;
	
	}

	/*
	*	Function:		void printDeviceCreateInfo(VkPhysicalDevice &device)
	*	Purpose:		Prints the device creation info
	*	
	*/
	// Physical device features
	VkPhysicalDeviceFeatures usedFeatures = {};
	// Device create info
	VkDeviceCreateInfo createInfo;
	void deviceCreateInfo(VkPhysicalDevice &device) {
	
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.flags = 0;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = NULL;
		createInfo.enabledExtensionCount = 0;
		createInfo.ppEnabledExtensionNames = NULL;
		createInfo.pEnabledFeatures = &usedFeatures;
	
	}

	/*
	*	Function:		void device()
	*	Purpose:		Creates the logical device from the physical device
	*
	*/
	void device() {

		// TODO: Pick best device instead of first device
		result = vkCreateDevice(physicalDevices[0], &createInfo, NULL, &logicalDevice);
		ASSERT_VULKAN(result);
	
	}

}

/*
*	Function:		int main()
*	Purpose:		Entry point for the application
*
*/
int main() {

	vulk::init();

	return 0;

}

