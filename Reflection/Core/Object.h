#pragma once

#include "ObjectType.h"
#include "ObjectTypeDictionary.h"
#include "ObjectMacro.h"
#include <type_traits>

namespace Reflection
{
	class CObject : public IToString
	{
		friend class CGarbageCollector;

	private:
		DECLARE_STATIC_CONSTRUCTOR()

	public:
		DECLARE_THIS_TYPE(CObject)

		/* DECLARE_SUPER_TYPE(CObject) */

		DECLARE_OBJECT_TYPE(virtual, )

		START_DECLARE_STATIC_OBJECT_TYPE()

		END_DECLARE_STATIC_OBJECT_TYPE(EObjectType::Class, CObject, , );

	public:
		CObject();

		void* operator new (size_t InSize);

		virtual ~CObject();

		virtual std::string ToString() const override;

	protected:
		virtual void GCDestructor() {}
	};
}

