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
            // 프로그램이 실행될 때 호출되는 클래스(구조체)의 함수를 정의합니다.
            // 이 함수는 모든 타입을 저장하는 컨테이너에 이 클래스(구조체의) 정보를 저장합니다.
            objectInfoBuilder.AppendLine(HeaderCfg.DeclareStaticConstructor());
            objectInfoBuilder.AppendLine("public:");
            // 이 타입이 구조체이면 new, delete 연산자를 제거하는 매크로를 작성합니다.
            objectInfoBuilder.AppendNonEmptyLine(HeaderCfg.DeleteStructNewDeleteOperators(objectInfo));
            // 이 타입, 이 타입의 부모를 정의합니다.
            objectInfoBuilder.AppendNonEmptyLine(HeaderCfg.DeclareThisType(objectInfo));
            objectInfoBuilder.AppendNonEmptyLine(HeaderCfg.DeclareSuperType(objectInfo));
            // 클래스(구조체)의 정보를 반환하는 함수를 정적 함수를 추가합니다.
            objectInfoBuilder.AppendLine(HeaderCfg.DeclareObjectType(objectInfo));
            // 클래스(구조체)의 정보를 반환하는 함수의 작성을 시작합니다.
            objectInfoBuilder.AppendLine(HeaderCfg.StartDeclareStaticObjectType());
            {
                foreach (Field field in objectInfo.fields)
                {
                    StringBuilder fieldBuilder = new StringBuilder();
                    // 멤버 변수의 정보를 추가하기 위한 여는 매크로를 작성합니다.
                    fieldBuilder.AppendLine(HeaderCfg.StartPushField());
                    {
                        // 멤버 변수가 템플릿 타입이면 템플릿 정보를 주입하는 매크로를 작성합니다.
                        PushTemplateParameters(field.templateParameters, fieldBuilder);
                        // 멤버 변수에 부착된 어트리뷰트, 전처리기 정보를 주입하는 매크로를 작성합니다.
                        PushMacroInfo(field.memberInfo, fieldBuilder);
                    }
                    // 멤버 변수의 정보를 추가하기 위한 닫는 매크로를 작성합니다.
                    // 여기서 멤버 변수에 접근(읽기, 쓰기)가능한 람다식을 표현하는 매크로를 작성합니다.
                    fieldBuilder.Append(HeaderCfg.EndPushField(field, objectInfo, newDefines));
                    // 멤버 변수가 전처리기에 감싸져 있던 경우,
                    // 이 멤버 변수의 정보를 추가하는 매크로를 전처리기로 감쌉니다. 
                    TryMacrolize($"FIELD_{field.name}", field.memberInfo, fieldBuilder);
                    objectInfoBuilder.AppendLine(fieldBuilder.ToString());
                }
                foreach (Function function in objectInfo.functions)
                {
                    StringBuilder functionBuilder = new StringBuilder();
                    // 멤버 함수의 정보를 추가하기 위한 여는 매크로를 작성합니다.
                    functionBuilder.AppendLine(HeaderCfg.StartPushFunction());
                    {
                        // 멤버 함수가 템플릿 타입이면 템플릿 정보를 주입하는 매크로를 작성합니다.
                        PushTemplateParameters(function.templateParameters, functionBuilder);
                        // 멤버 함수에 부착된 어트리뷰트, 전처리기 정보를 주입하는 매크로를 작성합니다.
                        PushMacroInfo(function.memberInfo, functionBuilder);
                        // 멤버 함수의 파라미터 정보를 주입하는 매크로를 작성합니다.
                        foreach (Parameter parameter in function.parameters)
                        {
                            functionBuilder.AppendLine(HeaderCfg.StartPushParameter());
                            {
                                PushTemplateParameters(parameter.templateParameters, functionBuilder);
                            }
                            functionBuilder.AppendLine(HeaderCfg.EndPushParameter(parameter, newDefines));
                        }
                    }
                    // 멤버 함수의 정보를 추가하기 위한 닫는 매크로를 작성합니다.
                    // 여기서 멤버 함수를 호출 가능한 람다식을 표현하는 매크로를 작성합니다.
                    functionBuilder.Append(HeaderCfg.EndPushFunction(function, objectInfo, newDefines));
                    // 멤버 함수가 전처리기에 감싸져 있던 경우,
                    // 이 멤버 함수의 정보를 추가하는 매크로를 전처리기로 감쌉니다. 
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
