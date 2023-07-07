using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace CPPReflection
{
    public static class Ut
    {
        public readonly static Regex findPreProcessorRegex = new Regex(@"#\s*((?#Command)elif|else|ifdef|ifndef|endif|if)\s*((?#Context)[a-zA-Z_][a-zA-Z0-9_]*)?");

        public static string RemoveAll(this string s, params string[] targets)
        {
            foreach (string target in targets)
            {
                s = s.Replace(target, "");
            }
            return s;
        }

        public static string RemoveEmptyWords(this string s)
        {
            return s.RemoveAll(" ", "\n", "\r", "\t");
        }

        public static bool Is(this char c, params char[] comparers)
        {
            foreach(char compare in comparers)
            {
                if(c == compare)
                {
                    return true;
                }
            }
            return false;
        }

        public static int ContainsCount(this string str, string value)
        {
            int n = 0;
            for (int i = str.IndexOf(value); i > -1; i = str.IndexOf(value, i + 1))
            {
                n++;
            }
            return n;
        }

        public static int ContainsCount(this string str, char value)
        {
            int n = 0;
            for (int i = str.IndexOf(value); i > -1; i = str.IndexOf(value, i + 1))
            {
                n++;
            }
            return n;
        }

        public static string Iteration(string str, int n)
        {
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < n; ++i)
            {
                sb.Append(str);
            }
            return sb.ToString();
        }

        public static string Iteration(char str, int n)
        {
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < n; ++i)
            {
                sb.Append(str);
            }
            return sb.ToString();
        }
    }
}
