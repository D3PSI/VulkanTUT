/*
*	Main.cpp: Defines the entry point for the console application.
*
*/
#include "vulkan/vulkan.h"
#include <iostream>
#include <intrin.h>

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

}

/*
*	Global Variables
*
*/
VkInstance instance;

/*
*	Namespace:		vulk
*	Purpose:		Prevent stupid naming conflicts
*
*/
namespace vulk {

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

		// Instance info
		VkInstanceCreateInfo instanceInfo;
		instanceInfo.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pNext						= NULL;
		instanceInfo.flags						= 0;
		instanceInfo.pApplicationInfo			= &appInfo;
		instanceInfo.enabledLayerCount			= 0;
		instanceInfo.ppEnabledLayerNames		= nullptr;
		instanceInfo.enabledExtensionCount		= 0;
		instanceInfo.ppEnabledExtensionNames	= nullptr;

		// Instance creation
		VkResult result = vkCreateInstance(

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

		VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];
		result = vkEnumeratePhysicalDevices(
			
			instance,						// Pass the instance
			&amountOfPhysicalDevices,		// Pass the amount of GPU's
			physicalDevices					// Pass the array to store the enumerated GPU's
		
		);
		ASSERT_VULKAN(result);

		std::cout << amountOfPhysicalDevices << std::endl;

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

