using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace CPPReflection
{
    public class Field : CPPReflection.Member
    {
        readonly static Regex fieldMacroRegex = new Regex(@"FIELD\([\s\S]*?\)");
        readonly static Regex fieldExtractRegex = new Regex(
            @"(?:((?#TypeConst)const)\s+)?
            (?:((?#ForwardType)class|struct)\s+)?
            ((?#TypeNamespaces)(?:[a-zA-Z_][a-zA-Z0-9_]*\s*::\s*)*)?
            ((?#Type)[a-zA-Z_][a-zA-Z0-9_]*)\s*
            (?:<(?#TemplateTypesBlock)([a-zA-Z0-9_:<>,\s\*]*)>)?\s*
            ((?#TypePointer)(?:\s*\*+)*)?\s*
            ((?#Reference)\&)?\s*
            ((?#Name)[a-zA-Z_][a-zA-Z0-9_]*)");

        private Field(NativeTypeInfo nativeType, List<TemplateParameter> templateParameters, string name, MemberInfo memberInfo) : 
            base(nativeType, templateParameters, name, memberInfo)
        {
        }

        public static bool GenerateFields(string code, ObjectType parentObjectType, out List<Field> fields)
        {
            fields = new List<Field>();

            foreach (Match fieldMacroMatch in fieldMacroRegex.Matches(code))
            {
                MemberInfo memberInfo;
                int searchStart;
                if(!AnalysisUt.ExtractMemberInfo(fieldMacroMatch, code, parentObjectType, out memberInfo, out searchStart))
                {
                    return false;
                }

                Match fieldExtractMatch = fieldExtractRegex.Match(code, searchStart);
                if (!fieldExtractMatch.Success)
                {
                    return false;
                }

                NativeTypeInfo nativeType;
                List<TemplateParameter> templateParameter;
                string name;
                if (!AnalysisUt.ExtractType(fieldExtractMatch, out nativeType, out templateParameter, out name))
                {
                    return false;
                }

                Field field = new Field(nativeType, templateParameter, name, memberInfo);
                fields.Add(field);
            }

            return true;
        }
    }

    public class Parameter : CPPReflection.Variable
    {
        readonly static Regex parameterExtractRegex = new Regex(@"(?:((?#TypeConst)const)\s+)?(?:((?#ForwardType)class|struct)\s+)?((?#TypeNamespaces)(?:[a-zA-Z_][a-zA-Z0-9_]*\s*::\s*)*)?((?#Type)[a-zA-Z_][a-zA-Z0-9_]*)\s*(?:<(?#TemplateTypesBlock)([a-zA-Z0-9_:<>,\s\*]*)>)?\s*((?#TypePointer)(?:\s*\*+)*)?\s*((?#Reference)\&)?\s*((?#Name)[a-zA-Z_][a-zA-Z0-9_]*)");

        public int index { get; private set; }

        private Parameter(NativeTypeInfo nativeType, List<TemplateParameter> templateParameters, string name, int index) :
            base(nativeType, templateParameters, name)
        {
            this.index = index;
        }

        public static bool GenerateParameters(string code, out List<Parameter> parameters)
        {
            parameters = new List<Parameter>();

            if (!RemoveDefaultParameters(ref code))
            {
                return false;
            }

            int index = 0;
            foreach (Match parameterExtractMatch in parameterExtractRegex.Matches(code))
            {
                if (!parameterExtractMatch.Success)
                {
                    return false;
                }

                NativeTypeInfo nativeType;
                List<TemplateParameter> templateParamters;
                string name;
                if (!AnalysisUt.ExtractType(parameterExtractMatch, out nativeType, out templateParamters, out name))
                {
                    return false;
                }

                Parameter parameter = new Parameter(nativeType, templateParamters, name, index++);
                parameters.Add(parameter);
            }

            return true;
        }

        static bool RemoveDefaultParameters(ref string code)
        {
            for (int i = code.IndexOf('='); i != -1; i = code.IndexOf('='))
            {
                int openStack = 0;
                for (int j = i + 1; j < code.Length; ++j)
                {
                    switch (code[j])
                    {
                        case '{':
                        case '}':
                        return false;

                        case '(':
                        case '[':
                        ++openStack;
                        break;

                        case ')':
                        case ']':
                        --openStack;
                        break;
                    }

                    if (openStack <= 0 && (code[j] == ',' || j == code.Length - 1))
                    {
                        string left = code.Substring(0, i);
                        string right = code.Substring(j);
                        code = left + right;
                        break;
                    }

                    // 디폴트 파라미터를 '='로 선언하고 끝맺음이 없는 문자열이면 유효하지 않은 문자열입니다.
                    if (j == code.Length - 1)
                    {
                        return false;
                    }
                }
            }
            return true;
        }
    }

    public class Function : CPPReflection.Member
    {
        readonly static Regex functionMacroRegex = new Regex(@"FUNCTION\([\s\S]*?\)");
        readonly static Regex functionExtractRegex = new Regex(@"(?:((?#TypeConst)const)\s+)?(?:((?#ForwardType)class|struct)\s+)?((?#TypeNamespaces)(?:[a-zA-Z_][a-zA-Z0-9_]*\s*::\s*)*)?((?#Type)[a-zA-Z_][a-zA-Z0-9_]*)\s*(?:<(?#TemplateTypesBlock)([a-zA-Z0-9_:<>,\s\*]*)>)?\s*((?#TypePointer)(?:\s*\*+)*)?\s*((?#Reference)\&)?\s*((?#Name)[a-zA-Z_][a-zA-Z0-9_]*)\s*((?#Paramters)\([a-zA-Z0-9\!\@\#\$\%\^\&\*\(\)\-_\=\+\[\{\]\}\:\'""\,\<\.\>\/\?\\\s]*\))(?:\s*((?#TypeConst)const))?");

        public List<Parameter> parameters { get; private set; }
        public bool isReadonlyFunction { get; private set; }

        private Function(NativeTypeInfo nativeType, List<TemplateParameter> templateParameters, string name, MemberInfo memberInfo, List<Parameter> parameters, bool isReadonlyFunction) :
            base(nativeType, templateParameters, name, memberInfo)
        {
            this.parameters = parameters;
            this.isReadonlyFunction = isReadonlyFunction;
        }

        public override string ToString()
        {
            string _parameters = "";
            for (int i = 0; i < parameters.Count; ++i)
            {
                _parameters += parameters[i].ToString();
                if (i < parameters.Count - 1)
                    _parameters += ", ";
            }
            string _isReadonlyFunction = isReadonlyFunction ? " const" : "";
            return $"{fullTypeString} {name}({_parameters}){_isReadonlyFunction}";
        }

        public static bool GenerateFunctions(string code, ObjectType parentObjectType, out List<Function> functions)
        {
            functions = new List<Function>();

            foreach (Match functionMacroMatch in functionMacroRegex.Matches(code))
            {
                MemberInfo memberInfo;
                int searchStart;
                if (!AnalysisUt.ExtractMemberInfo(functionMacroMatch, code, parentObjectType, out memberInfo, out searchStart))
                {
                    return false;
                }

                Match functionExtractMatch = functionExtractRegex.Match(code, searchStart);
                if (!functionExtractMatch.Success)
                {
                    return false;
                }

                NativeTypeInfo nativeType;
                List<TemplateParameter> templateParameters;
                string name;
                if (!AnalysisUt.ExtractType(functionExtractMatch, out nativeType, out templateParameters, out name))
                {
                    return false;
                }

                Parenthesis parenthesis;
                if(!Parenthesis.Extract(code, functionExtractMatch.Groups[8].Index, '(', ')', out parenthesis))
                {
                    return false;
                }
                List<Parameter> parameters;
                if (!Parameter.GenerateParameters(parenthesis.content, out parameters))
                {
                    return false;
                }
                bool isReadonlyFunction = functionExtractMatch.Groups[10].Value.Contains("const");

                Function function = new Function(nativeType, templateParameters, name, memberInfo, parameters, isReadonlyFunction);
                functions.Add(function);
            }

            return true;
        }
    }

    public class ObjectInfo
    {
        #region REGULAR_EXPRESSIONS
        readonly static Regex objectMacroRegex = new Regex(@"OBJECT\([\s\S]*?\)");
        readonly static Regex objectExtractRegex = new Regex(@"((?#ObjectType)class|struct)\s+(?:((?#API)[a-zA-Z_][a-zA-Z0-9_]*API)\s+)?((?#Name)[CS][a-zA-Z0-9_]*)\s*:?\s*(?:((?#SuperAccess)public|protected|private)\s+)?\s*((?#SuperNamespaces)(?:[a-zA-Z_][a-zA-Z0-9_]*\s*::\s*)*)?((?#Super)[a-zA-Z_][a-zA-Z0-9_]*)?");
        readonly static Regex middleParenthesesRegex = new Regex(@"[{}]");
        readonly static Regex findMarkedReflectionMacroRegex = new Regex(@"REFLECTION\s*\([\s\S]*?\)\[@@(?#LineNumber)(\d*)@@\]");
        #endregion
        public MacroInfo macroInfo { get; private set; }
        public List<Field> fields { get; private set; }
        public List<Function> functions { get; private set; }
        public string code { get; private set; }
        public string modifiedPath { get; private set; }
        public int reflectionMacroLine { get; private set; }
        public ObjectType type { get; private set; }
        public string name { get; private set; }
        public string super { get; private set; }
        public string superNamespaces { get; private set; }

        private ObjectInfo(MacroInfo macroInfo, List<Field> fields, List<Function> functions, ObjectType type, string name, string super, string superNamespaces, string code, string modifiedPath, int reflectionMacroLine)
        {
            this.macroInfo = macroInfo;
            this.fields = fields;
            this.functions = functions;
            this.type = type;
            this.name = name;
            this.super = super;
            this.superNamespaces = superNamespaces;
            this.code = code;
            this.modifiedPath = modifiedPath;
            this.reflectionMacroLine = reflectionMacroLine;
        }

        public override string ToString()
        {
            return $"{type} {name}{(super != "" ? $" : {super}" : "")}";
        }

        public static bool GenerateObjects(string fullCode, string modifiedPath, out List<ObjectInfo> objects)
        {
            objects = new List<ObjectInfo>();

            CodeModifier.ModifyCode(ref fullCode);

            int objectStart = 0;
            foreach (Match objectMacroMatch in objectMacroRegex.Matches(fullCode, objectStart))
            {
                MacroInfo macroInfo;
                int searchStart;
                if (!AnalysisUt.ExtractMacroInfo(objectMacroMatch, fullCode, out macroInfo, out searchStart))
                {
                    return false;
                }

                Match objectExtractMatch = objectExtractRegex.Match(fullCode, searchStart);
                if (!objectExtractMatch.Success)
                {
                    return false;
                }

                objectStart = objectExtractMatch.Index;
                searchStart = fullCode.IndexOf('{', objectExtractMatch.Index + objectExtractMatch.Value.Length) + 1;
                if (searchStart == -1)
                {
                    return false;
                }

                int stack = 1;
                while (stack > 0)
                {
                    Match middleParenthesesMatch = middleParenthesesRegex.Match(fullCode, searchStart);
                    if (!middleParenthesesMatch.Success)
                    {
                        return false;
                    }
                    searchStart = middleParenthesesMatch.Index + 1;

                    if (middleParenthesesMatch.Value == "{")
                    {
                        ++stack;
                    }
                    else
                    {
                        --stack;
                    }
                    if (stack == 0)
                    {
                        string code = fullCode.Substring(objectStart, middleParenthesesMatch.Index - objectStart + 1);

                        Match findReflectionMacroMatch = findMarkedReflectionMacroRegex.Match(code);
                        if (!findReflectionMacroMatch.Success)
                        {
                            return false;
                        }

                        int reflectionMacroLine;
                        if (!int.TryParse(findReflectionMacroMatch.Groups[1].Value, out reflectionMacroLine))
                        {
                            return false;
                        }

                        ObjectType objectType = AnalysisUt.ParseObjectType(objectExtractMatch.Groups[1].Value);
                        string name = objectExtractMatch.Groups[3].Value;
                        string superNamespaces = objectExtractMatch.Groups[5].Value;
                        string super = objectExtractMatch.Groups[6].Value;

                        List<Field> fields;
                        if (!Field.GenerateFields(code, objectType, out fields))
                        {
                            return false;
                        }

                        List<Function> functions;
                        if (!Function.GenerateFunctions(code, objectType, out functions))
                        {
                            return false;
                        }

                        ObjectInfo objectInfo = new ObjectInfo(macroInfo, fields, functions, objectType, name, super, superNamespaces, code, modifiedPath, reflectionMacroLine);
                        objects.Add(objectInfo);

                        objectStart = middleParenthesesMatch.Index + middleParenthesesMatch.Value.Length;
                        break;
                    }
                }
            }
            return true;
        }
    }
}
