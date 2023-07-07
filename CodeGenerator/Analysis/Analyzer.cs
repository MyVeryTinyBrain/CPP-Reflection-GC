using System;
using System.Text;
using System.Text.RegularExpressions;

namespace CPPReflection
{
    public class Analyzer
    {
        public string filePath { get; private set; }
        public string modifiedFilePath { get; private set; }
        public string fileName { get; private set; }
        public string fileNameWithoutExtension { get; private set; }
        public List<ObjectInfo> objects { get; private set; }

        private Analyzer(string filePath, string modifiedPath, List<ObjectInfo> objects)
        {
            this.filePath = filePath;
            this.modifiedFilePath = modifiedPath;
            fileName = Path.GetFileName(filePath);
            fileNameWithoutExtension = Path.GetFileNameWithoutExtension(fileName);

            this.objects = objects;
        }

        public static Analyzer AnalyzeCode(string filePath)
        {
            string code = File.ReadAllText(filePath);
            string modifiedFilePath = AnalysisUt.ModifyFilePath(filePath);

            List<ObjectInfo> objects;
            ObjectInfo.GenerateObjects(code, modifiedFilePath, out objects);

            return new Analyzer(filePath, modifiedFilePath, objects);
        }
    }
}