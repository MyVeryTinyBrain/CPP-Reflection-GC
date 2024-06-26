﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace CPPReflection
{
    public static class AnalysisUt
    {
        public static string ModifyFilePath(string fileName)
        {
            return Regex.Replace(fileName, @"[!@#$%^&()\-=+\[{\]}:;',./\s\\]", "_");
        }

        public static bool ExtractConents(string str, int startIndex, out List<string> contents)
        {
            contents = new List<string>();
            str = str.RemoveEmptyWords();

            int stack = 0, contentStartIndex = 0;
            for (int i = startIndex; i < str.Length; ++i)
            {
                if (str[i].Is('(', '{', '['))
                {
                    ++stack;
                }
                else if (str[i].Is(')', '}', ']'))
                {
                    --stack;
                    if (stack < 0)
                    {
                        return false;
                    }
                }
                if (stack == 0 && (str[i].Is(',') || i == str.Length - 1))
                {
                    int offset = (i == str.Length - 1) ? 1 : 0;
                    string content = str.Substring(contentStartIndex, i - contentStartIndex + offset);
                    contents.Add(content);
                    contentStartIndex = i + 1;
                    if(contentStartIndex >= str.Length)
                    {
                        break;
                    }
                }
            }

            return true;
        }

        public static bool ExtractConents(Parenthesis parenthesis, out List<string> contents)
        {
            return ExtractConents(parenthesis.content, 0, out contents);
        }

        public static int FindNextOpenIndex(string str, int startIndex)
        {
            for(int i = startIndex; i < str.Length; ++i)
            {
                if (!str[i].Is(')', '}', ']', ';', '\n', '\r', '\t', ' '))
                {
                    return i;
                }
            }
            return -1;
        }

        public static bool ExtractAttributes(string str, int startIndex, out List<string> attributes, out int attributeEndIndex)
        {
            attributes = new List<string>();
            attributeEndIndex = -1;

            Parenthesis parenthesis;
            if(!Parenthesis.Extract(str, startIndex, '(', ')', out parenthesis))
            {
                return false;
            }

            if(!ExtractConents(parenthesis, out attributes))
            {
                return false;
            }

            attributeEndIndex = FindNextOpenIndex(str, parenthesis.closeParenthesisIndex);
            return true;
        }

        public static ObjectType ParseObjectType(string typeName)
        {
            if (typeName == "class")
            {
                return ObjectType.Class;
 
            }
            else if (typeName == "struct")
            {
                return ObjectType.Struct;
            }
            else
            {
                return ObjectType.None;
            }
        }

        public static bool ExtractMacroInfo(Match macroMatch, string code, out MacroInfo macroInfo, out int searchStart)
        {
            macroInfo = new MacroInfo();
            macroInfo.preProcessors = PreProcessor.GetPreProcessors(code, macroMatch.Index);
            if (!ExtractAttributes(code, macroMatch.Index, out macroInfo.attributes, out searchStart))
            {
                return false;
            }
            return true;
        }

        public static bool ExtractMemberInfo(Match macroMatch, string code, ObjectType parentObjectType, out MemberInfo memberInfo, out int searchStart)
        {
            memberInfo = new MemberInfo();
            memberInfo.preProcessors = PreProcessor.GetPreProcessors(code, macroMatch.Index);
            memberInfo.accessType = MemberAccess.GetAccessType(code, macroMatch.Index, parentObjectType);
            if (!ExtractAttributes(code, macroMatch.Index, out memberInfo.attributes, out searchStart))
            {
                return false;
            }
            return true;
        }

        public static bool ExtractType(Match extractMatch, out NativeTypeInfo nativeType, out List<TemplateParameter> templateTypes, out string name)
        {
            nativeType = NativeTypeInfo.Invalid;
            name = "";
            // 이 멤버변수가 상수인지 아닌지를 저장합니다.
            bool isConst = extractMatch.Groups[1].Value.Contains("const");
            // 이 멤버변수가 전방선언 된 타입이면 어떤 종류인지 저장합니다.
            ObjectType forward = AnalysisUt.ParseObjectType(extractMatch.Groups[2].Value);
            // 이 멤버변수가 네임스페이스를 가지면 네임스페이스를 저장합니다.
            string namespaces = extractMatch.Groups[3].Value.Replace(" ", "");
            // 이 멤버변수의 타임을 탐색합니다.
            string typeName = extractMatch.Groups[4].Value;
            // 이 멤버변수가 템플릿이면 모든 템플릿 타입을 탐색합니다.
            if (!TemplateParameter.Extract(extractMatch.Groups[5].Value, out templateTypes))
            {
                return false;
            }
            // * 기호의 개수를 저장합니다.
            int numPointers = extractMatch.Groups[6].Value.ContainsCount('*');
            // 이 멤버변수가 레퍼런스 타입인지 검사합니다.
            bool isReference = extractMatch.Groups[7].Value.Contains('&');

            nativeType = new NativeTypeInfo(isConst, forward, namespaces, typeName, numPointers, isReference);
            name = extractMatch.Groups[8].Value;

            return true;
        }
    }

    public class Parenthesis
    {
        public int openParenthesisIndex { get; private set; }
        public int closeParenthesisIndex { get; private set; }
        public string parenthesis { get; private set; }
        public string content { get; private set; }

        Parenthesis(int openParenthesisIndex, int closeParenthesisIndex, string parenthesis, string content)
        {
            this.openParenthesisIndex = openParenthesisIndex;
            this.closeParenthesisIndex = closeParenthesisIndex;
            this.parenthesis = parenthesis;
            this.content = content;
        }

        Parenthesis() : this(-1, -1, "", "")
        {
        }

        public static bool Extract(string str, int startIndex, char open, char close, out Parenthesis parenthesis)
        {
            parenthesis = new Parenthesis();

            for (int i = startIndex; i < str.Length; ++i)
            {
                if (str[i] == open)
                {
                    parenthesis.openParenthesisIndex = i;
                    break;
                }
            }

            if (parenthesis.openParenthesisIndex == -1)
            {
                return false;
            }

            int stack = 1;
            for (int i = parenthesis.openParenthesisIndex + 1; i < str.Length; ++i)
            {
                if (str[i] == open)
                {
                    ++stack;
                }
                else if (str[i] == close)
                {
                    --stack;
                    if (stack == 0)
                    {
                        parenthesis.closeParenthesisIndex = i;
                        break;
                    }
                    else if (stack < 0)
                    {
                        return false;
                    }
                }
            }

            int parenthesisLength = parenthesis.closeParenthesisIndex - parenthesis.openParenthesisIndex + 1;
            int contentLength = Math.Max(0, parenthesisLength - 2);
            parenthesis.parenthesis = str.Substring(parenthesis.openParenthesisIndex, parenthesisLength);
            parenthesis.content = str.Substring(parenthesis.openParenthesisIndex + 1, contentLength);
            return true;
        }
    }

    public enum PreProcessorType
    {
        Invalid, If, Elif, Else, Ifdef, Ifndef, Endif
    }

    public class PreProcessor
    {
        public string commandString { get; private set; }
        public PreProcessorType command { get; private set; }
        public string context { get; private set; }

        private PreProcessor(string commandString, string context)
        {
            this.commandString = commandString;
            this.context = context;

            if (commandString == "if")
            {
                command = PreProcessorType.If;
            }
            else if (commandString == "elif")
            {
                command = PreProcessorType.Elif;
            }
            else if (commandString == "else")
            {
                command = PreProcessorType.Else;
            }
            else if (commandString == "ifdef")
            {
                command = PreProcessorType.Ifdef;
            }
            else if (commandString == "ifndef")
            {
                command = PreProcessorType.Ifndef;
            }
            else if (commandString == "endif")
            {
                command = PreProcessorType.Endif;
            }
            else
            {
                command = PreProcessorType.Invalid;
            }
        }
        public override string ToString()
        {
            return $"#{commandString} {context}";
        }

        // 코드 문자열의 at 번째 문자까지의 모든 전처리기를 검사합니다.
        // 닫힌 전처리기는 무시하고, 열린 전처리기를 모두 반환합니다.
        public static List<string> GetPreProcessors(string code, int at)
        {
            LinkedList<PreProcessor> preProcessors = new LinkedList<PreProcessor>();
            Match match = Ut.findPreProcessorRegex.Match(code, 0, at + 1);

            // 탐색한 전처리기가 at 이전에 존재하면 반복합니다.
            while (match.Success && match.Index < at)
            {
                PreProcessor preProcessor = new PreProcessor(match.Groups[1].Value, match.Groups[2].Value);
                // 이 매크로가 닫는 전처리기(endif)이면
                if (preProcessor.command == PreProcessorType.Endif)
                {
                    // 여는 전처리기(if)를 포함한, 전까지의 모든 전처리기를 제거합니다.
                    bool keepContinue = true;
                    while (preProcessors.Count > 0 && preProcessors.Last != null && keepContinue)
                    {
                        switch (preProcessors.Last.Value.command)
                        {
                            case PreProcessorType.If:
                            case PreProcessorType.Ifdef:
                            case PreProcessorType.Ifndef:
                            keepContinue = false;
                            break;
                        }
                        preProcessors.RemoveLast();
                    }
                }
                else
                {
                    preProcessors.AddLast(preProcessor);
                }
                // 다음 전처리기를 탐색합니다.
                match = match.NextMatch();
            }

            // 열린 전처리기 문자열을 저장하고, 반환합니다.
            List<string> preProcessorLines = new List<string>();
            foreach (PreProcessor preProcessor in preProcessors)
            {
                preProcessorLines.Add(preProcessor.ToString());
            }
            return preProcessorLines;
        }
    }

    public class MemberAccess
    {
        readonly static Regex findAccessTypeRegex = new Regex(@"((?:public)|(?:protected)|(?:private))\s*:");

        private MemberAccess()
        {
        }

        public static AccessType GetAccessType(string code, int at, ObjectType objectType)
        {
            AccessType defaultAccess;
            switch (objectType)
            {
                case ObjectType.Struct:
                defaultAccess = AccessType.Public;
                break;

                default:
                defaultAccess = AccessType.Private;
                break;
            }
            return GetAccessType(code, at, defaultAccess);
        }

        public static AccessType GetAccessType(string code, int at, AccessType defaultType)
        {
            Match match = findAccessTypeRegex.Match(code, 0, at + 1);
            Func<string, AccessType> ToAccessType = (str) =>
            {
                if (str.Contains("public"))
                {
                    return AccessType.Public;
                }
                else if (str.Contains("protected"))
                {
                    return AccessType.Protected;
                }
                else if (str.Contains("private"))
                {
                    return AccessType.Private;
                }
                else
                {
                    return defaultType;
                }
            };
            AccessType lastType = defaultType;
            while (match.Success && match.Index < at)
            {
                lastType = ToAccessType(match.Value);
                match = match.NextMatch();
            }
            return lastType;
        }
    }

    public class CodeModifier
    {
        readonly static Regex addLineNumberRegex = new Regex(@"\n");
        readonly static Regex findLineNumberRegex = new Regex(@"\[##(?#LineNumber)(\d*)##\]\n");
        readonly static Regex removeCommentsRegex = new Regex(@"(@(?:[^\n\\]+|\\.)*(?:[^\n\\]+|\\.)*)|//.*|/\*(?s:.*?)\*/");
        readonly static Regex markReflectionMacroRegex = new Regex(@"REFLECTION\s*\([\s\S]*?\)");

        public static bool ModifyCode(ref string code)
        {
            // 코드의 각 줄마다 줄 번호를 마킹한다.
            string linedCode = AddLineNumber(code);
            string markedReflectionMacroCode;
            // REFLECTION() 매크로가 있는 줄에 마킹한다.
            if (!MarkReflectionMacro(linedCode, out markedReflectionMacroCode))
            {
                return false;
            }
            // REFLECTION() 매크로가 있는 줄을 제외하고 줄 번호 마킹을 제거한다.
            string unlinedCode = RemoveLineNumber(markedReflectionMacroCode);
            // 주석을 제거한다.
            string uncommentedCode = RemoveComments(unlinedCode);
            code = uncommentedCode;
            return true;
        }

        static bool MarkReflectionMacro(string linedCode, out string markedReflectionMacroCode)
        {
            bool isValid = true;
            markedReflectionMacroCode = "";

            MatchEvaluator evaluator = (Match match) =>
            {
                Match findLineNumberMatch = findLineNumberRegex.Match(linedCode, match.Index + match.Value.Length);
                int lineNumber = 0;
                if (!int.TryParse(findLineNumberMatch.Groups[1].Value, out lineNumber))
                {
                    isValid = false;
                }
                return $"{match.Value}[@@{lineNumber}@@]";
            };

            markedReflectionMacroCode = markReflectionMacroRegex.Replace(linedCode, evaluator);
            return isValid;
        }

        static string AddLineNumber(string code)
        {
            int lineNumber = 1;
            MatchEvaluator evaluator = (Match match) =>
            {
                return $"[##{(lineNumber++)}##]\n";
            };
            return addLineNumberRegex.Replace(code, evaluator);
        }

        static string RemoveLineNumber(string code)
        {
            return findLineNumberRegex.Replace(code, "\n");
        }

        static string RemoveComments(string code)
        {
            return removeCommentsRegex.Replace(code, "$1");
        }
    }
}
