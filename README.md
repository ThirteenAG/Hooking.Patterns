Hooking.Patterns
----------------
Sample:
    
    #include "stdafx.h"
    #include <Windows.h>
    #include "Hooking.Patterns.h"
    
    int main()
    {
        auto pattern = hook::pattern("54 68 69 73 20 70 72 6F 67 72");
        if (pattern.size() > 0)
        {
            auto text = pattern.get(0).get<char>(0);
            MessageBoxA(0, text, text,0);
        }
        return 0;
    }

Result:

![messagebox](http://i.imgur.com/Tuijf2I.png)
