using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WorldEditor.Base.Services
{
    // 实现Service最基本的接口
    class StandardServiceBaseImpl : Interfaces.Services.IService
    {
        public virtual string Name
        {
            get { return this.GetType().ToString(); }
        }

        public virtual bool Initialize()
        {
            return true;
        }

        public virtual void Destroy()
        {

        }

        public virtual bool Start()
        {
            return true;
        }

        public virtual bool End()
        {
            return true;
        }

        public virtual void Update()
        {

        }
    }
}
