using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CPPReflection
{
    public class HeaderGenerator
    {
        Analyzer analyzer;
        NewDefines newDefines;
        StringBuilder builder;
        StringBuilder objectBuilder;

        private HeaderGenerator(Analyzer analyzer)
        {
            this.analyzer = analyzer;
            newDefines = new NewDefines();
            builder = new StringBuilder();
            objectBuilder = new StringBuilder();
        }

        public override string ToString()
        {
            return builder.ToString();
        }

        public static HeaderGenerator Generate(Analyzer analyzer)
        {
            HeaderGenerator generator = new HeaderGenerator(analyzer);

            generator.WriteObjectMacros();

            generator.PragmaOnce();
            generator.DefineFileName();
            generator.DefineNewMacros();

            generator.DefineObjectMacros();

            return generator;
        }

        void PragmaOnce()
        {
            builder.AppendLine("#pragma once");
            builder.AppendLine();
        }

        void DefineFileName()
        {
            builder.AppendLine(HeaderCfg.DefineFileName(analyzer));
            builder.AppendLine();
        }

        void DefineNewMacros()
        {
            if (!newDefines.empty)
            {
                builder.AppendLine(newDefines.ToString());
                builder.AppendLine();
            }
        }

        void WriteObjectMacros()
        {
            for (int i = 0; i < analyzer.objects.Count; ++i)
            {
                DefineObjectMacro(analyzer.objects[i]);
                if (i < analyzer.objects.Count - 1)
                {
                    objectBuilder.AppendLine();
                }
            }
        }

        void DefineObjectMacros()
        {
            builder.AppendLine(objectBuilder.ToString());
        }

        void PushTemplateParameters(List<TemplateParameter> templateParameters, StringBuilder builder)
        {
            foreach (TemplateParameter templateParameter in templateParameters)
            {
                builder.AppendLine(HeaderCfg.PushTemplateParameter(templateParameter));
            }
        }

        void PushMacroInfo(MacroInfo macroInfo, StringBuilder builder)
        {
            foreach (string preProcessor in macroInfo.preProcessors)
            {
                builder.AppendLine(HeaderCfg.PushPreProcessor(preProcessor));
            }
            foreach (string attribute in macroInfo.attributes)
            {
                builder.AppendLine(HeaderCfg.PushAttribute(attribute));
            }
        }

        string DefaultAccess(ObjectInfo objectInfo)
        {
            switch (objectInfo.type)
            {
                default:
                case ObjectType.Class:
                return "private:";

                case ObjectType.Struct:
                return "public:";
            }
        }

        void TryMacrolize(string macroName, MacroInfo macroInfo, StringBuilder builder)
        {
            if (macroInfo.preProcessors.Count > 0)
            {
                newDefines.MakeMultilineDefine(ref macroName, builder.ToString(), macroInfo.preProcessors);
                builder.Clear();
                builder.Append(macroName);
            }
        }

        void DefineObjectMacro(ObjectInfo objectInfo)
        {
            StringBuilder objectInfoBuilder = new StringBuilder();
            objectInfoBuilder.AppendLine("private:");
            objectInfoBuilder.AppendLine(HeaderCfg.DeclareStaticConstructor());
            objectInfoBuilder.AppendLine("public:");
            objectInfoBuilder.AppendNonEmptyLine(HeaderCfg.DeleteStructNewDeleteOperators(objectInfo));
            objectInfoBuilder.AppendNonEmptyLine(HeaderCfg.DeclareThisType(objectInfo));
            objectInfoBuilder.AppendNonEmptyLine(HeaderCfg.DeclareSuperType(objectInfo));
            objectInfoBuilder.AppendLine(HeaderCfg.DeclareObjectType(objectInfo));
            objectInfoBuilder.AppendLine(HeaderCfg.StartDeclareStaticObjectType());
            {
                foreach (Field field in objectInfo.fields)
                {
                    StringBuilder fieldBuilder = new StringBuilder();
                    fieldBuilder.AppendLine(HeaderCfg.StartPushField());
                    {
                        PushTemplateParameters(field.templateParameters, fieldBuilder);
                        PushMacroInfo(field.memberInfo, fieldBuilder);
                    }
                    fieldBuilder.Append(HeaderCfg.EndPushField(field, objectInfo, newDefines));
                    TryMacrolize($"FIELD_{field.name}", field.memberInfo, fieldBuilder);
                    objectInfoBuilder.AppendLine(fieldBuilder.ToString());
                }
                foreach (Function function in objectInfo.functions)
                {
                    StringBuilder functionBuilder = new StringBuilder();
                    functionBuilder.AppendLine(HeaderCfg.StartPushFunction());
                    {
                        PushTemplateParameters(function.templateParameters, functionBuilder);
                        PushMacroInfo(function.memberInfo, functionBuilder);
                        foreach (Parameter parameter in function.parameters)
                        {
                            functionBuilder.AppendLine(HeaderCfg.StartPushParameter());
                            {
                                PushTemplateParameters(parameter.templateParameters, functionBuilder);
                            }
                            functionBuilder.AppendLine(HeaderCfg.EndPushParameter(parameter, newDefines));
                        }
                    }
                    functionBuilder.Append(HeaderCfg.EndPushFunction(function, objectInfo, newDefines));
                    TryMacrolize($"FUNCTION_{function.name}", function.memberInfo, functionBuilder);
                    objectInfoBuilder.AppendLine(functionBuilder.ToString());
                }
                PushMacroInfo(objectInfo.macroInfo, objectInfoBuilder);
            }
            objectInfoBuilder.AppendNonEmptyLine(HeaderCfg.SetupDeleteLambda(objectInfo));
            objectInfoBuilder.AppendLine(HeaderCfg.EndDeclareStaticObjectType(objectInfo));
            objectInfoBuilder.Append(DefaultAccess(objectInfo));
            TryMacrolize($"OBJECT_{objectInfo.name}", objectInfo.macroInfo, objectInfoBuilder);

            string objectMacroName = HeaderCfg.ObjectMacroName(objectInfo);
            string macro = GeneratorUt.MultilineDefine(objectMacroName, objectInfoBuilder.ToString());
            objectBuilder.AppendLine(macro);
        }
    }
}
