using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WorldEditor.Base.Interfaces.MVC
{
    // 文档接口
    public interface IDocument
    {
        // 文档是否被修改
        bool Modified { set; get; }

        // 文档是否能修改
        bool CanModify { set; get; }

        // 文档文件名
        String Key { get;}

        // 文档处理
        // @{
        bool New(String szKey);
        bool Open(String szKey);
        bool Close();
        bool Save();
        bool SaveAs(String szKey);
        // @}

        // 事件委托
        // @{
        event EventHandler<EventArgs> DataChanged;
        event EventHandler<DocumentEventArgs> PreNew;
        event EventHandler<DocumentEventArgs> PostNew;
        event EventHandler<DocumentEventArgs> PreOpen;
        event EventHandler<DocumentEventArgs> PostOpen;
        event EventHandler<DocumentEventArgs> PreClose;
        event EventHandler<DocumentEventArgs> PostClose;
        event EventHandler<DocumentEventArgs> PreSave;
        event EventHandler<DocumentEventArgs> PostSave;
        event EventHandler<DocumentEventArgs> PreSaveAs;
        event EventHandler<DocumentEventArgs> PostSaveAs;
        // @}
    }

    // 文档事件参数
    public class DocumentEventArgs : EventArgs
    {
        private readonly String m_szKey;
        public String Key { get { return m_szKey; } }

        public DocumentEventArgs(String szKey) { m_szKey = szKey; }
    }
}
