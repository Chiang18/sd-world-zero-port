using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WorldEditor.Base.Interfaces
{
    // 插件接口
    interface IPlugin
    {
        // 插件加载
        bool Load(int iMajorVersion, int iMinorVersion);

        // 插件启动
        bool Start();

        // 插件服务列表
        Services.IService[] GetProvidedServices();
    }
}
