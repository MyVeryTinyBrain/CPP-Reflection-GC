using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CPPReflection
{
    public class CodeGenerator
    {
        bool lastWriteTimeCheck;
        bool objectCheck;
        bool log;
        SourceGenerator sourceGenerator;
        List<string> headerFilePathes;
        List<Analyzer> analyzers;

        string GenerateDirectory
        {
            get
            {
                return Path.Combine(Directory.GetCurrentDirectory(), "Reflection");
            }
        }

        string ReflectionHeaderExtension
        {
            get
            {
                return ".reflection.h";
            }
        }

        string ReflectionSourceName
        {
            get
            {
                return "_reflection_source_";
            }
        }

        string ReflectionSourceExtension
        {
            get
            {
                return ".reflection.cpp";
            }
        }

        CodeGenerator(string[] args)
        {
            lastWriteTimeCheck = true;

            objectCheck = true;
            log = true;
            sourceGenerator = new SourceGenerator();
            headerFilePathes = new List<string>();
            analyzers = new List<Analyzer>();

            ApplyArguments(args);

            Directory.CreateDirectory(GenerateDirectory);
        }

        static void Main(string[] args)
        {
            CodeGenerator codeGenerator = new CodeGenerator(args);
            codeGenerator.CollectHeaderFilePathes();
            codeGenerator.Analyze();
            codeGenerator.Generate();
        }

        bool TryParseBool(string arg)
        {
            int result;
            if (int.TryParse(arg, out result))
            {
                return (result > 0);
            }
            return true;
        }

        void ApplyArguments(string[] args)
        {
            ArgumentCommand command = ArgumentCommand.None;
            foreach (string arg in args)
            {
                if (command == ArgumentCommand.None)
                {
                    command = CodeGeneratorUt.Parse(arg);
                }
                else
                {
                    switch (command)
                    {
                        case ArgumentCommand.AbsoluteDirectory:
                        Directory.SetCurrentDirectory(arg);
                        break;

                        case ArgumentCommand.RelativeDirectory:
                        Directory.SetCurrentDirectory(Path.GetRelativePath(Directory.GetCurrentDirectory(), arg));
                        break;

                        case ArgumentCommand.LastWriteTimeCheck:
                        lastWriteTimeCheck = TryParseBool(arg);
                        break;

                        case ArgumentCommand.ObjectCheck:
                        objectCheck = TryParseBool(arg);
                        break;

                        case ArgumentCommand.Log:
                        log = TryParseBool(arg);
                        break;
                    }
                    command = ArgumentCommand.None;
                }
            }
        }

        void CollectHeaderFilePathes()
        {
            headerFilePathes.Clear();
            Internal_CollectHeaderFilePathes(Directory.GetCurrentDirectory(), 0);
        }

        void Internal_CollectHeaderFilePathes(string directory, int step)
        {
            string[] headerFilesInDirectory = Directory.GetFiles(directory, "*.h");
            foreach (string headerFilePath in headerFilesInDirectory)
            {
                if (!headerFilePath.Contains(ReflectionHeaderExtension))
                {
                    headerFilePathes.Add(Path.GetRelativePath(Directory.GetCurrentDirectory(), headerFilePath));
                }
            }

            string[] childDirectories = Directory.GetDirectories(directory);
            foreach (string childDirectory in childDirectories)
            {
                if (step == 0 && childDirectory == GenerateDirectory)
                {
                    continue;
                }
                Internal_CollectHeaderFilePathes(childDirectory, step + 1);
            }
        }

        string ReflectionHeaderFilePath(string headerFilePath)
        {
            return Path.Combine(GenerateDirectory, Path.GetFileNameWithoutExtension(headerFilePath)) + ReflectionHeaderExtension;
        }

        string ReflectionSourceFilePath()
        {
            return Path.Combine(GenerateDirectory, ReflectionSourceName) + ReflectionSourceExtension;
        }

        bool CanGenerateHeaderFile(string headerFilePath)
        {
            if (!lastWriteTimeCheck)
            {
                return true;
            }

            string reflectionHeaderFilePath = ReflectionHeaderFilePath(headerFilePath);
            if (File.Exists(reflectionHeaderFilePath))
            {
                DateTime headerLastWriteTime = File.GetLastWriteTime(headerFilePath);
                DateTime reflectionHeaderLastWriteTime = File.GetLastWriteTime(reflectionHeaderFilePath);
                if (headerLastWriteTime == reflectionHeaderLastWriteTime)
                {
                    return false;
                }
            }
            return true;
        }

        void Log(string message)
        {
            if (log)
            {
                Console.WriteLine($"[Reflection]{message}");
            }
        }

        void Analyze()
        {
            List<Thread> threads = new List<Thread>();
            int headerFileIndex = 0;
            Mutex headerFileIndexMutex = new Mutex();
            Mutex analyzersMutex = new Mutex();

            for(int i = 0; i < Environment.ProcessorCount; ++i)
            {
                Thread thread = new Thread(() => AnalyzeJob(ref headerFileIndex, headerFileIndexMutex, analyzersMutex));
                thread.Start();
                threads.Add(thread);
            }

            foreach (Thread thread in threads)
            {
                thread.Join();
            }
        }

        void AnalyzeJob(ref int headerFileIndex, Mutex headerFileIndexMutex, Mutex analyzersMutex)
        {
            while (true)
            {
                string headerFilePath;
                lock (headerFileIndexMutex)
                {
                    if (headerFileIndex >= headerFilePathes.Count)
                    {
                        return;
                    }
                    headerFilePath = headerFilePathes[headerFileIndex++];
                }

                Analyzer analyzer = Analyzer.AnalyzeCode(headerFilePath);
                if (objectCheck && analyzer.objects.Count == 0)
                {
                    continue;
                }

                lock (analyzersMutex)
                {
                    analyzers.Add(analyzer);
                }
            }
        }

        void Generate()
        {
            sourceGenerator.Clear();

            List<Thread> threads = new List<Thread>();
            int analyzersIndex = 0;
            List<string> logs = new List<string>();
            int numGenerated = 0, numSkipped = 0;
            Mutex analyzersIndexMutex = new Mutex();
            Mutex sourceGeneratorMutex = new Mutex();
            Mutex logMutex = new Mutex();

            for (int i = 0; i < Environment.ProcessorCount; ++i)
            {
                Thread thread = new Thread(() => GenerateJob(ref analyzersIndex, ref numGenerated, ref numSkipped, logs, analyzersIndexMutex, sourceGeneratorMutex, logMutex));
                thread.Start();
                threads.Add(thread);
            }

            foreach (Thread thread in threads)
            {
                thread.Join();
            }

            GenerateSource(ref numGenerated, ref numSkipped, logs);

            foreach(string log in logs)
            {
                Log(log);
            }
            Log($"[Result]Skipped: {numSkipped}");
            Log($"[Result]Generated: {numGenerated}");
        }

        void GenerateJob(ref int analyzersIndex, ref int numGenerated, ref int numSkipped, List<string> logs, Mutex analyzersIndexMutex, Mutex sourceGeneratorMutex, Mutex logMutex)
        {
            while (true)
            {
                Analyzer analyzer = null;
                lock (analyzersIndexMutex)
                {
                    if(analyzersIndex >= analyzers.Count)
                    {
                        return;
                    }
                    analyzer = analyzers[analyzersIndex++];
                }

                lock (sourceGeneratorMutex)
                {
                    sourceGenerator.Append(analyzer);
                }

                string reflectionHeaderFilePath = ReflectionHeaderFilePath(analyzer.filePath);

                if (!CanGenerateHeaderFile(analyzer.filePath))
                {
                    lock (logMutex)
                    {
                        logs.Add($"[Skipped][Header]{reflectionHeaderFilePath}");
                        numSkipped++;
                    }
                    continue;
                }
                
                File.WriteAllTextAsync(reflectionHeaderFilePath, HeaderGenerator.Generate(analyzer).ToString());
                File.SetLastWriteTime(reflectionHeaderFilePath, File.GetLastWriteTime(analyzer.filePath));
                lock (logMutex)
                {
                    logs.Add($"[Generated][Header]{reflectionHeaderFilePath}");
                    numGenerated++;
                }
            }
        }

        void GenerateSource(ref int numGenerated, ref int numSkipped, List<string> logs)
        {
            string reflectionSourceFilePath = ReflectionSourceFilePath();

            File.WriteAllTextAsync(reflectionSourceFilePath, sourceGenerator.ToString());

            logs.Add($"[Generated][Source]{reflectionSourceFilePath}");
            numGenerated++;
        }
    }

    public enum ArgumentCommand
    {
        None,
        AbsoluteDirectory,
        RelativeDirectory,
        LastWriteTimeCheck,
        ObjectCheck,
        Log,
    }

    public static class CodeGeneratorUt
    {
        public static ArgumentCommand Parse(string argument)
        {
            argument = argument.ToLower();
            if (argument == "-absolutedirectory")
            {
                return ArgumentCommand.AbsoluteDirectory;
            }
            else if (argument == "-relativedirectory")
            {
                return ArgumentCommand.RelativeDirectory;
            }
            else if (argument == "-lastwritetimecheck")
            {
                return ArgumentCommand.LastWriteTimeCheck;
            }
            else if(argument == "-objectcheck")
            {
                return ArgumentCommand.ObjectCheck;
            }
            else if(argument == "-log")
            {
                return ArgumentCommand.Log;
            }
            return ArgumentCommand.None;
        }
    }
}
