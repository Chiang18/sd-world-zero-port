using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WorldEditor.Base.Interface
{
    // 服务接口
    interface IService
    {

        // 服务初始化
        bool Initialize();

        // 服务启动
        bool Start();
    }
}
