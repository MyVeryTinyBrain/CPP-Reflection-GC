using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CPPReflection
{
    public static class GeneratorUt
    {
        public static string ToCPPString(this bool b)
        {
            return b ? "true" : "false";
        }

        public static string ToCPPString(this ObjectType objectType)
        {
            switch (objectType)
            {
                case ObjectType.Class:
                return "EObjectType::Class";

                case ObjectType.Struct:
                return "EObjectType::Struct";

                default:
                return "EObjectType::None";
            }
        }

        public static string ToCPPString(this AccessType accessType)
        {
            switch (accessType)
            {
                case AccessType.Public:
                return "EAccessType::Public";

                case AccessType.Protected:
                return "EAccessType::Protected";

                case AccessType.Private:
                return "EAccessType::Private";

                default:
                return "EAccessType::Private";
            }
        }

        public static string ToCPPBool(this bool b)
        {
            return b ? "true" : "false";
        }

        public static string GetCPPDefaultAccess(this ObjectInfo objectInfo)
        {
            return (objectInfo.type == ObjectType.Class ? "private:" : "public:");
        }

        public static string SinglelineDefine(string name, string context)
        {
            return
                $"#ifdef {name} \n" +
                $"#undef {name} \n" +
                $"#endif \n" +
                $"#define {name} {context}";
        }

        public static string MultilineDefine(string name, string context)
        {
            StringBuilder builder = new StringBuilder();

            builder.AppendLine($"#ifdef {name}");
            builder.AppendLine($"#undef {name}");
            builder.AppendLine($"#endif");
            builder.AppendLine($"#define {name} \\");

            context = context.Replace("\\", "");
            string[] lines = context.Split('\n', StringSplitOptions.TrimEntries);

            for (int i = 0; i < lines.Length; i++)
            {
                if (i < lines.Length - 1)
                {
                    builder.AppendLine($"{lines[i]} \\");
                }
                else
                {
                    builder.Append($"{lines[i]}");
                }
            }

            return builder.ToString();
        }

        public static string CombineAttributes(List<string> attributes)
        {
            StringBuilder builder = new StringBuilder();
            builder.Append("{");

            for (int i = 0; i < attributes.Count; ++i)
            {
                builder.Append($"new {attributes[i]}");
                if (i < attributes.Count - 1)
                    builder.Append(", ");
            }

            builder.Append("}");
            return builder.ToString();
        }

        public static string Sizeof(string typeName)
        {
            if (typeName.RemoveEmptyWords() == "void")
            {
                return "0";
            }
            else
            {
                return $"sizeof({typeName})";
            }
        }

        public static StringBuilder AppendNonEmptyLine(this StringBuilder builder, string value)
        {
            if (value.RemoveEmptyWords().Length == 0)
            {
                return builder;
            }
            builder.AppendLine(value);
            return builder;
        }
    }

    public class NewDefines
    {
        List<string> defines;
        Dictionary<string, int> names;

        public bool empty => defines.Count == 0;

        public NewDefines()
        {
            defines = new List<string>();
            names = new Dictionary<string, int>();
        }

        public override string ToString()
        {
            StringBuilder builder = new StringBuilder();

            for (int i = 0; i < defines.Count; ++i)
            {
                builder.AppendLine($"{defines[i]}");
                if (i < defines.Count - 1)
                {
                    builder.AppendLine();
                }
            }

            return builder.ToString();
        }

        string MakeDefineName(string name)
        {
            int count = 0;
            if (!names.TryGetValue(name, out count))
            {
                names.Add(name, 0);
            }
            names[name] = count + 1;
            return $"___{name}_{count}___";
        }

        public void MakeSinglelineDefine(ref string name, string context)
        {
            MakeDefine_Internal(ref name, context, false);
        }

        public void MakeMultilineDefine(ref string name, string context)
        {
            MakeDefine_Internal(ref name, context, true);
        }

        public void MakeSinglelineDefine(ref string name, string context, List<string> preProcessors)
        {
            if (preProcessors.Count == 0)
            {
                MakeDefine_Internal(ref name, context, false);
            }
            else
            {
                MakePreProcessorsDefine_Internal(ref name, context, preProcessors, false);
            }
        }

        public void MakeMultilineDefine(ref string name, string context, List<string> preProcessors)
        {
            if (preProcessors.Count == 0)
            {
                MakeDefine_Internal(ref name, context, true);
            }
            else
            {
                MakePreProcessorsDefine_Internal(ref name, context, preProcessors, true);
            }
        }

        void MakeDefine_Internal(ref string name, string context, bool multiline)
        {
            name = MakeDefineName(name);
            string macro;
            if (multiline)
            {
                macro = GeneratorUt.MultilineDefine(name, context);
            }
            else
            {
                macro = GeneratorUt.SinglelineDefine(name, context);
            }
            defines.Add(macro);
        }

        void MakePreProcessorsDefine_Internal(ref string name, string context, List<string> preProcessors, bool multiline)
        {
            StringBuilder builder = new StringBuilder();

            name = MakeDefineName(name);
            builder.AppendLine(GeneratorUt.SinglelineDefine(name, ""));

            foreach (string preProcessor in preProcessors)
            {
                builder.AppendLine(preProcessor);
            }

            if (multiline)
            {
                builder.AppendLine(GeneratorUt.MultilineDefine(name, context));
            }
            else
            {
                builder.AppendLine(GeneratorUt.SinglelineDefine(name, context));
            }

            for (int i = 0; i < preProcessors.Count; i++)
            {
                builder.Append("#endif");
                if (i < preProcessors.Count - 1)
                {
                    builder.AppendLine();
                }
            }

            defines.Add(builder.ToString());
        }
    }
}
