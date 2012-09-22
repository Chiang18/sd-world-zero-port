using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WorldEditor.Base
{
    // 单件
    public class TSingleton<T> where T : new ()
    {
        private static T instance;
        public static T Instance
        {
            get
            {
                if (instance == null)
                {
                    try
                    {
                        instance = new T();
                    }
                    catch (System.Exception ex)
                    {
                        System.Windows.Forms.MessageBox.Show(ex.ToString());
                    }
                }

                return instance;
            }
        }

        protected TSingleton() { }
    }
}
