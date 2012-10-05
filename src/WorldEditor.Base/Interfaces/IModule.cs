using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WorldEditor.Base.Interfaces
{
    // 编辑模块接口
    interface IModule
    {

        // 初始化与销毁模块
        bool Initialize();
        void Destroy();

        // 进入与退出模块
        bool Enter();
        void Leave();

        // 文档处理

    }
}
