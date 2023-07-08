# C++ GC With Reflection
언리얼의 Reflection, GC와 유사한 방식으로 구현한 API입니다.

## 프로젝트 구성
>### CodeGenerator
>>#### Root/CodeGenerator
>>>C#으로 제작된 코드 분석 및 코드 생성기입니다.  
>>#### Root/CodeGenerator.zip
>>>릴리즈 버전으로 빌드된 파일과 구성요소가 있습니다.  
>### Reflection
>>#### Root/Reflection/CodeGenerator
>>>CodeGenerator.zip의 압축이 해제되어 있습니다.  
>>>Reflection프로젝트는 빌드 전에 이 파일을 실행하게 됩니다.  
>>#### Root/Reflection/Core
>>>C++ 리플렉션이 구현되어 있습니다.  
>>>다른 프로젝트에서 사용하려면 이 폴더의 내용물만 다른 프로젝트에 옮기면 됩니다.  
>>#### Root/Reflection/Reflection
>>>CodeGenerator.exe가 생성한 폴더입니다.  
>>>>내부에는 CodeGenerator.exe가 생성한 코드들이 포함되어 있습니다.  
>>>이 폴더와 내부의 파일들은 CodeGenerator.exe가 필요에 의해 생성하므로 삭제해도 무관합니다.  
>>#### Root/Reflection
>>>리플렉션과 GC를 사용하는 샘플 코드입니다.

## 프로젝트에 적용하기
> 1. 적용하려는 프로젝트 폴더의 적당한 위치에 CodeGenerator.zip의 압축을 풉니다.
> 2. 프로젝트의 빌드 전 이벤트에서 CodeGenerator.exe를 실행하도록 합니다.
> 3. 프로젝트를 한 번 빌드합니다.  
> 4. 프로젝트 내부에 Reflection 폴더가 생성되었다면 _reflection_source_.reflection.cpp 파일을 프로젝트에 포함시킵니다.

## 시작 명령어
>빌드 전 이벤트에서 CodeGenerator.exe를 실행하는 명령문을 남길 때 명령어들을 같이 전달해 줄 수 있습니다.  
>명령어는 대소문자를 구분하지 않습니다.  
>### -AbsoluteDirectory "directory"
>> 절대 경로를 지정하여 해당 경로부터 탐색을 시작하도록 합니다.
> 또한 해당 경로에 Reflection 폴더가 생성되고 이 폴더 내부에 코드들이 생성됩니다.
> 기본값은 CodeGenerator.exe를 실행하는 프로젝트의 경로입니다.
>### - RelativeDirectory - "directory"
>> CodeGenerator.exe가 위치한 경로에 대한 상대 경로를 지정합니다.
>### -LastWriteTimeCheck 0|1
>> 0: 마지막으로 수정한 시간 검사를 하지 않습니다.  
>> 1: 헤더 파일과 리플렉션 헤더 파일의 마지막으로 수정한 시간이 같으면 변경 사항이 없다고 판단하여 리플렉션 헤더 파일을 생성하지 않습니다.  
>> 기본값은 1입니다.  
>### -ObjectCheck 0|1
>> 0: 헤더 파일에 리플렉션 정보를 생성할 오브젝트가 정의되어 있지 않아도 리플렉션 헤더 파일을 생성합니다.  
>> 1: 헤더 파일에 리플렉션 정보를 생성할 오브젝트가 정의되어 있지 않으면 리플렉션 헤더 파일을 생성하지 않습니다.  
>> 기본값은 1입니다.
>### -Log 0|1
>> 0: 콘솔에 아무것도 표시하지 않습니다.  
>> 1: 진행 상황 및 결과를 콘솔에 표시합니다.  
>> 기본값은 1입니다.  

## 코드 작성하기
> ### 헤더파일 포함 규칙
>> CodeGenerator.exe가 생성한 "헤더파일이름.reflection.h" 파일을 포함해야 합니다.  
>> 이 파일에 리플렉션 데이터가 포함되어 있습니다.  
>> 만약 TestTypes.h 라는 파일에 리플렉션을 적용하려면 TestTypes.reflection.h 파일을 포함해야 합니다.   
>> 또한 .reflection.h 파일은 모든 include 명령의 가장 마지막에 위치해야 합니다.  
> ### 정의하는 타입에 대하여 리플렉션 데이터 생성하기
>> 구조체의 이름은 S로 시작해야 하며 클래스의 이름은 C로 시작해야 합니다.  
>> 선언한 구조체 또는 클래스의 위쪽 줄에 OBJECT() 매크로를 추가합니다.  
>> 타입 내부에 REFLECTION() 매크로를 추가합니다.  
> ### 멤버 변수를 리플렉션 데이터에 추가하기
>> 대상 멤버 변수 위쪽 줄에 FIELD() 매크로를 추가합니다.
> ### 멤버 함수를 리플렉션 데이터에 추가하기
>> 대상 멤버 함수 위쪽 줄에 FUNCTION() 매크로를 추가합니다.