using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace CPPReflection
{
    public class SourceGenerator
    {
        Analyzer analyzer;
        StringBuilder builder;

        public SourceGenerator()
        {
            builder = new StringBuilder();
            analyzer = null;
        }

        public override string ToString()
        {
            return builder.ToString();
        }

        public void Clear()
        {
            builder.Clear();
        }

        public void Append(Analyzer analyzer)
        {
            if (analyzer == null)
            {
                return;
            }
            this.analyzer = analyzer;

            IncludeHeader();
            ImplementStaticConstructor();
            builder.AppendLine();
        }

        void IncludeHeader()
        {
            builder.AppendLine(SourceCfg.IncludeHeader(analyzer));
        }

        void ImplementStaticConstructor()
        {
            foreach (ObjectInfo objectInfo in analyzer.objects)
            {
                PutInPreProcessors(SourceCfg.ImplementStaticConstructor(objectInfo), objectInfo.macroInfo.preProcessors);
            }
        }

        void PutInPreProcessors(string context, List<string> preProcessors)
        {
            foreach (string preProcessor in preProcessors)
            {
                builder.AppendLine(preProcessor);
            }

            builder.AppendLine(context);

            foreach (string preProcessor in preProcessors)
            {
                builder.AppendLine("#endif");
            }
        }
    }
}
