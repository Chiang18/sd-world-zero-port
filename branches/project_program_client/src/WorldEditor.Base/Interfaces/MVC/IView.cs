using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WorldEditor.Base.Interface.MVC
{
    // 视图接口
    interface IView
    {
        // 视图所属文档
        IDocument Document { get; }

        // 视图的文档适配器
        IViewAdapter ViewAdapter { get; set; }
    }
}
