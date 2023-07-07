using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CPPReflection
{
    public static class HeaderCfg
    {
        public static string DefineFileName(Analyzer analyzer)
        {
            return GeneratorUt.SinglelineDefine("___FILE_NAME___", analyzer.modifiedFilePath);
        }

        public static string ObjectMacroName(ObjectInfo objectInfo)
        {
            return $"___REFLECTION_{objectInfo.modifiedPath}_{objectInfo.reflectionMacroLine}___";
        }

        public static string DeclareThisType(ObjectInfo objectInfo)
        {
            return $"DECLARE_THIS_TYPE({objectInfo.name})";
        }

        public static string DeclareSuperType(ObjectInfo objectInfo)
        {
            if(objectInfo.super == "")
            {
                return "";
            }
            else
            {
                return $"DECLARE_SUPER_TYPE({objectInfo.super})";
            }
        }

        public static string DeclareObjectType(ObjectInfo objectInfo)
        {
            switch (objectInfo.type)
            {
                case ObjectType.Class:
                return $"DECLARE_CLASS_OBJECT_TYPE()";

                default:
                case ObjectType.Struct:
                return $"DECLARE_STRUCT_OBJECT_TYPE()";
            }
        }

        public static string DeclareStaticConstructor()
        {
            return $"DECLARE_STATIC_CONSTRUCTOR()";
        }

        public static string PushPreProcessor(string context)
        {
            return $"PUSH_PRE_PROCESSOR({context})";
        }

        public static string PushAttribute(string context)
        {
            return $"PUSH_ATTRIBUTE({context})";
        }

        private static string NativeTypeInfoArguments(NativeTypeInfo nativeTypeInfo)
        {
            NativeTypeInfo t = nativeTypeInfo;
            string fullTypeName = t.ToString(true, true, true, true, true);
            return $"{t.isConst.ToCPPString()}, {t.forward.ToCPPString()}, {t.namespaces}, {t.typeName}, {t.numPointers}, {t.isReference.ToCPPString()}, {fullTypeName}, {GeneratorUt.Sizeof(fullTypeName)}";
        }

        private static string TypeArguments(CPPReflection.Type type, NewDefines newDefines, out string fullTypeName)
        {
            CPPReflection.Type ty = type;
            NativeTypeInfo t = ty.typeInfo;
            fullTypeName = ty.fullTypeString;
            if (type.isTemplate)
            {
                string macroName = "TEMPLATE_TYPE";
                newDefines.MakeSinglelineDefine(ref macroName, fullTypeName);
                fullTypeName = macroName;
            }
            return $"{t.isConst.ToCPPString()}, {t.forward.ToCPPString()}, {t.namespaces}, {t.typeName}, {t.numPointers}, {t.isReference.ToCPPString()}, {fullTypeName}, {GeneratorUt.Sizeof(fullTypeName)}";
        }

        public static string PushTemplateParameter(TemplateParameter templateParameter)
        {
            return $"PUSH_TEMPLATE_PARAMETER({NativeTypeInfoArguments(templateParameter.typeInfo)}, {templateParameter.index}, {templateParameter.isUnknownType.ToCPPString()})";
        }

        public static string StartDeclareStaticObjectType()
        {
            return $"START_DECLARE_STATIC_OBJECT_TYPE()";
        }

        public static string EndDeclareStaticObjectType(ObjectInfo objectInfo)
        {
            return $"END_DECLARE_STATIC_OBJECT_TYPE({objectInfo.type.ToCPPString()}, {objectInfo.name}, {objectInfo.super}, {objectInfo.superNamespaces})";
        }

        private static string SetupFieldGetLambda(string fullTypeName, string name, ObjectInfo declaringObject)
        {
            return $"SETUP_FIELD_GET_LAMBDA({declaringObject.name}, {fullTypeName}, {name})";
        }
        
        private static string SetupFieldSetLambda(string fullTypeName, string name, ObjectInfo declaringObject)
        {
            return $"SETUP_FIELD_SET_LAMBDA({declaringObject.name}, {fullTypeName}, {name})";
        }

        public static string StartPushField()
        {
            return $"START_PUSH_FIELD()"; 
        }

        public static string EndPushField(Field field, ObjectInfo declaringObject, NewDefines newDefines)
        {
            StringBuilder builder = new StringBuilder();

            string fullTypeName;
            string typeArguments = TypeArguments(field, newDefines, out fullTypeName);

            builder.AppendLine($"{SetupFieldGetLambda(fullTypeName, field.name, declaringObject)}");

            if (field.typeInfo.isConst == false)
            {
                builder.AppendLine($"{SetupFieldSetLambda(fullTypeName, field.name, declaringObject)}");
            }

            builder.Append($"END_PUSH_FIELD({typeArguments}, {field.memberInfo.accessType.ToCPPString()}, {declaringObject.name}, {field.name})");
            
            return builder.ToString();
        }

        public static string StartPushParameter()
        {
            return $"START_PUSH_PARAMETER()";
        }

        public static string EndPushParameter(Parameter parameter, NewDefines newDefines)
        {
            string fullTypeName;
            string typeArguments = TypeArguments(parameter, newDefines, out fullTypeName);
            return $"END_PUSH_PARAMETER({typeArguments}, {parameter.name}, {parameter.index})";
        }

        private static string PlaceFunctionCallLambdaParameter(Parameter parameter)
        {
            return $"PLACE_FUNCTION_CALL_LAMBDA_PARAMETER({parameter.GetTypeString(true,true,true,true,false)}, {parameter.index})";
        }

        private static string SetupFunctionCallLambda(Function function, ObjectInfo objectInfo, NewDefines newDefines)
        {
            StringBuilder builder = new StringBuilder();

            string fullReturnTypeName = function.GetTypeString(false, true, true, true, false);
            if (function.isTemplate)
            {
                string macroName = "TEMPLATE_FUNCTION_CALL_RETURN_TYPE";
                newDefines.MakeSinglelineDefine(ref macroName, fullReturnTypeName);
                fullReturnTypeName = macroName;
            }

            if (function.typeString == "void")
            {
                builder.AppendLine($"START_SETUP_FUNCTION_CALL_NORETURN_LAMBDA({fullReturnTypeName}, {function.name}, {objectInfo.name})");
            }
            else
            {
                builder.AppendLine($"START_SETUP_FUNCTION_CALL_RETURN_LAMBDA({fullReturnTypeName}, {function.name}, {objectInfo.name})");
            }

            for (int i = 0; i < function.parameters.Count; ++i)
            {
                builder.Append(PlaceFunctionCallLambdaParameter(function.parameters[i]));

                if (i < function.parameters.Count - 1)
                {
                    builder.Append(',');
                }

                builder.AppendLine();
            }

            if (function.typeString == "void")
            {
                builder.Append($"END_SETUP_FUNCTION_CALL_NORETURN_LAMBDA()");
            }
            else
            {
                builder.Append($"END_SETUP_FUNCTION_CALL_RETURN_LAMBDA()");
            }

            return builder.ToString();
        }

        public static string StartPushFunction()
        {
            return $"START_PUSH_FUNCTION()";
        }

        public static string EndPushFunction(Function function, ObjectInfo declaringObject, NewDefines newDefines)
        {
            StringBuilder builder = new StringBuilder();

            string fullTypeName;
            string typeArguments = TypeArguments(function, newDefines, out fullTypeName);

            builder.AppendLine(SetupFunctionCallLambda(function, declaringObject, newDefines));

            builder.Append($"END_PUSH_FUNCTION({typeArguments}, {function.memberInfo.accessType.ToCPPString()}, {declaringObject.name}, {function.name}, {function.isReadonlyFunction.ToCPPString()})");

            return builder.ToString();
        }

        public static string SetupDeleteLambda(ObjectInfo objectInfo)
        {
            if(objectInfo.type == ObjectType.Class)
            {
                return $"SETUP_DELETE_LAMBDA({objectInfo.name})";
            }
            return "";
        }

        public static string DeleteStructNewDeleteOperators(ObjectInfo objectInfo)
        {
            if(objectInfo.type == ObjectType.Struct)
            {
                return $"DELETE_STRUCT_NEW_DELETE_OPERATORS()";
            }
            return "";
        }
    }

    public static class SourceCfg
    {
        public static string IncludeHeader(Analyzer analyzer)
        {
            return @$"#include ""../{analyzer.filePath}""";
        }

        public static string ImplementStaticConstructor(ObjectInfo objectInfo)
        {
            return $"{objectInfo.name}::SStaticConstructor {objectInfo.name}::StaticConstructor;";
        }
    }
}
