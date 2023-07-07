#pragma once

namespace Reflection
{
	class CGarbageCollector;
	class CObjectTypeDictionary;

	class CReflectionStaticManager
	{
		struct SStaticConstructor
		{
			SStaticConstructor();
			~SStaticConstructor();
		};;
		static SStaticConstructor StaticConstructor;
	};
}

