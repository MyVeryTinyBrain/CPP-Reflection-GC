using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace CPPReflection
{
    public enum ObjectType
    {
        None, Class, Struct
    }

    public enum AccessType
    {
        Private, Protected, Public
    }

    public class NativeTypeInfo
    {
        public bool isConst { get; private set; }
        public ObjectType forward { get; private set; }
        public string namespaces { get; private set; }
        public string typeName { get; private set; }
        public int numPointers { get; private set; }
        public bool isReference { get; private set; }

        public static NativeTypeInfo Invalid => new NativeTypeInfo(false, ObjectType.None, "", "", 0, false);
        public bool isValid => typeName != "";
        public string pointers => Ut.Iteration('*', numPointers);
        public string reference => isReference ? "&" : "";

        public NativeTypeInfo(bool isConst, ObjectType forward, string namespaces, string name, int numPointers, bool isReference)
        {
            this.isConst = isConst;
            this.forward = forward;
            this.namespaces = namespaces.RemoveEmptyWords();
            this.typeName = name;
            this.numPointers = numPointers;
            this.isReference = isReference;
        }

        public string ToString(bool includeConst, bool includeForward, bool includeNamespaces, bool includePointers, bool includeReference)
        {
            string isConstString = (includeConst && isConst) ? "const " : "";
            string forwardString = (includeForward && forward != ObjectType.None) ? $"{forward.ToCPPString()} " : "";
            string namespacesString = (includeNamespaces) ? namespaces : "";
            string pointersString = (includePointers) ? pointers : "";
            string referenceString = (includeReference) ? reference : "";
            return $"{isConstString}{forwardString}{namespacesString}{typeName}{pointersString}{referenceString}";
        }

        public override string ToString()
        {
            return ToString(true, false, false, true, true);
        }
    }

    public abstract class NativeType
    {
        public NativeTypeInfo typeInfo { get; private set; }
        public bool isValid => typeInfo.isValid;

        public NativeType(NativeTypeInfo typeInfo)
        {
            this.typeInfo = typeInfo;
        }

        public virtual string ToString(bool includeConst, bool includeForward, bool includeNamespaces, bool includePointers, bool includeReference)
        {
            return typeInfo.ToString(includeConst, includeForward, includeNamespaces, includePointers, includeReference);
        }

        public override string ToString()
        {
            return typeInfo.ToString();
        }
    }

    public class TemplateParameter : CPPReflection.NativeType
    {
        readonly static Regex templateTypeExtractRegex = new Regex(@"(?:((?#TypeConst)const)\s+)?(?:((?#ForwardType)class|struct)\s+)?((?#TypeNamespaces)(?:[a-zA-Z_][a-zA-Z0-9_]*\s*::\s*)*)?((?#TypeOrValue)[a-zA-Z0-9_]+)\s*(?:<(?#TemplateTypesBlock)([a-zA-Z0-9_:<>,\s\*]*)>)?\s*((?#TypePointer)(?:\s*\*+)*)?\s*((?#Reference)\&)?\s*");

        public int index { get; private set; }
        public bool isUnknownType { get; private set; }

        public TemplateParameter(NativeTypeInfo typeInfo, int index, bool isUnknownType) :
            base(typeInfo)
        {
            this.index = index;
            this.isUnknownType = isUnknownType;
        }

        public static bool Extract(string templateTypesBlock, out List<CPPReflection.TemplateParameter> templateParameters)
        {
            templateParameters = new List<TemplateParameter>();

            List<string> contents;
            if (!AnalysisUt.ExtractConents(templateTypesBlock, 0, out contents))
            {
                return false;
            }

            for (int i = 0; i < contents.Count; ++i)
            {
                Match templateTypeExtractMatch = templateTypeExtractRegex.Match(contents[i]);
                if (!templateTypeExtractMatch.Success)
                {
                    return false;
                }

                bool _const = templateTypeExtractMatch.Groups[1].Value.Contains("const");
                ObjectType _forward = AnalysisUt.ParseObjectType(templateTypeExtractMatch.Groups[2].Value);
                string _namespaces = templateTypeExtractMatch.Groups[3].Value.Replace(" ", "");
                string _typeOrValueString = templateTypeExtractMatch.Groups[4].Value;
                string _templateTypesBlock = templateTypeExtractMatch.Groups[5].Value.RemoveEmptyWords();
                int _numPointers = templateTypeExtractMatch.Groups[6].Value.ContainsCount('*');
                bool _isReference = templateTypeExtractMatch.Groups[7].Value.Contains('&');

                int typeOrValueStartIndex = AnalysisUt.FindNextOpenIndex(_typeOrValueString, 0);
                if(typeOrValueStartIndex == -1)
                {
                    return false;
                }

                TemplateParameter templateParameter;
                if (_typeOrValueString[typeOrValueStartIndex].Is('0', '1', '2', '3', '4', '5', '6', '7', '8', '9'))
                {
                    NativeTypeInfo nativeTypeInfo = new NativeTypeInfo(_const, _forward, _namespaces, _typeOrValueString.RemoveEmptyWords(), _numPointers, _isReference);
                    templateParameter = new TemplateParameter(nativeTypeInfo, i, true);
                }
                else if(_templateTypesBlock.Length > 0)
                {
                    string _typeWithTemplate = $"{_typeOrValueString}<{_templateTypesBlock}>";
                    NativeTypeInfo nativeTypeInfo = new NativeTypeInfo(_const, _forward, _namespaces, _typeWithTemplate.RemoveEmptyWords(), _numPointers, _isReference);
                    templateParameter = new TemplateParameter(nativeTypeInfo, i, true);
                }
                else
                {
                    NativeTypeInfo nativeTypeInfo = new NativeTypeInfo(_const, _forward, _namespaces, _typeOrValueString.RemoveEmptyWords(), _numPointers, _isReference);
                    templateParameter = new TemplateParameter(nativeTypeInfo, i, false);
                }
                templateParameters.Add(templateParameter);
            }

            return true;
        }
    }

    public abstract class Type : CPPReflection.NativeType
    {
        public List<TemplateParameter> templateParameters { get; private set; }
        public bool isTemplate => templateParameters.Count > 0;
        public string typeString => GetTypeString(true, false, false, true, true);
        public string fullTypeString => GetTypeString(true, true, true, true, true);

        public Type(NativeTypeInfo typeInfo, List<TemplateParameter> templateParameters) : 
            base(typeInfo)
        {
            this.templateParameters = templateParameters;
        }

        public bool IsUnknown()
        {
            foreach(TemplateParameter templateParameter in templateParameters)
            {
                if(templateParameter.isUnknownType)
                {
                    return true;
                }
            }
            return false;
        }

        public string TemplateTypesString(bool includeConst, bool includeForward, bool includeNamespaces, bool includePointers, bool includeReference)
        {
            StringBuilder templateTypesStringBuilder = new StringBuilder();
            for (int i = 0; i < templateParameters.Count; ++i)
            {
                templateTypesStringBuilder.Append(templateParameters[i].ToString(includeConst, includeForward, includeNamespaces, includePointers, includeReference));
                if (i < templateParameters.Count - 1)
                {
                    templateTypesStringBuilder.Append(", ");
                }
            }
            return templateTypesStringBuilder.ToString();
        }

        public string GetTypeString(bool includeConst, bool includeForward, bool includeNamespaces, bool includePointers, bool includeReference)
        {
            string typeWithoutPointersAndReferenceString = typeInfo.ToString(includeConst, includeForward, includeNamespaces, false, false);
            string typePointersString = "";
            if (includePointers)
            {
                typePointersString = typeInfo.pointers;
            }
            string typeReferenceString = "";
            if (includeReference)
            {
                typeReferenceString = typeInfo.reference;
            }
            string templateTypesString = "";
            if (templateParameters.Count > 0)
            {
                templateTypesString = $"<{TemplateTypesString(includeConst, includeForward, includeNamespaces, includePointers, includeReference)}>";
            }
            return $"{typeWithoutPointersAndReferenceString}{templateTypesString}{typePointersString}{typeReferenceString}";
        }

        public override string ToString(bool includeConst, bool includeForward, bool includeNamespaces, bool includePointers, bool includeReference)
        {
            return GetTypeString(includeConst, includeForward, includeNamespaces, includePointers, includeReference);
        }

        public override string ToString()
        {
            return typeString;
        }
    }

    public abstract class Variable : CPPReflection.Type
    {
        public string name { get; private set; }

        public string variableString => ToString(true, false, false, true, true);
        public string fullVariableName => ToString(true, true, true, true, true);

        public Variable(NativeTypeInfo typeInfo, List<TemplateParameter> templateParameters, string name) : 
            base(typeInfo, templateParameters)
        {
            this.name = name;
        }

        public override string ToString(bool includeConst, bool includeForward, bool includeNamespaces, bool includePointers, bool includeReference)
        {
            string typeString = base.ToString(includeConst, includePointers, includeNamespaces, includePointers, includeReference);
            return $"{typeString} {name}";
        }

        public override string ToString()
        {
            return variableString;
        }
    }

    public class MacroInfo
    {
        public List<string> preProcessors;
        public List<string> attributes;

        public MacroInfo()
        {
            this.preProcessors = new List<string>();
            this.attributes = new List<string>();
        }
    }

    public class MemberInfo : MacroInfo
    {
        public AccessType accessType;

        public MemberInfo()
        {
            this.accessType = AccessType.Public;
        }
    }

    public class Member : CPPReflection.Variable
    {
        public MemberInfo memberInfo { get; private set; }

        public string memberName => ToString(true, false, false, true, true);
        public string fullMemberName => ToString(true, true, true, true, true);

        public Member(NativeTypeInfo nativeType, List<TemplateParameter> templateParameter, string name, MemberInfo memberInfo) : 
            base(nativeType, templateParameter, name)
        {
            this.memberInfo = memberInfo;
        }

        public string ToString(bool includeConst, bool includeForward, bool includeNamespaces, bool includePointers, bool includeAccessType, bool includeReference)
        {
            string typeString = base.ToString(includeConst, includePointers, includeNamespaces, includePointers, includeReference);
            string accessTypeString = "";
            if (includeAccessType)
            {
                switch (memberInfo.accessType)
                {
                    case AccessType.Public:
                    accessTypeString = "public";
                    break;

                    case AccessType.Protected:
                    accessTypeString = "protected";
                    break;

                    case AccessType.Private:
                    accessTypeString = "private";
                    break;
                }
            }
            return $"{accessTypeString} {typeString}";
        }

        public override string ToString()
        {
            return memberName;
        }
    }
}