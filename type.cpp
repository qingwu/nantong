#include <QtCore/QString>
#include "type.h"

Type::Type(/*QString *parent*/) //:
    /*QString(parent)*/
{
}
Type::~Type()
{
}

QString Type::char_shijian(int shijian_index, int danyuan_index)
{
    switch (shijian_index)
    {
        case 0x01:
            return QString("  正  常  ");
        break;
        case 0x02:
        {
            if ((danyuan_index >= 0x01) && (danyuan_index <= 0x04))
            {
                return QString("稳态值超限");
            }
            else if ((danyuan_index >= 0x05) && (danyuan_index <= 0x06))
            {
                return QString("触点开路  ");
            }
            else
            {
                return QString(" 24V故障  ");
            }
        }
        break;
        case 0x03:
        {
            if ((danyuan_index >= 0x10) && (danyuan_index <= 0x11))
            {
                return QString("短路隔离  ");
            }
            else
            {
                return QString("总线欠压  ");
            }
        }
        break;
        case 0x04:
        {
            return QString("输出故障  ");
        }
        break;
        case 0x05:
        {
            return QString("线路#2开路");
        }
        break;
        case 0x06:
        {
            return QString("输入故障  ");
        }
        break;
        case 0x07:
        {
            return QString("线路短路  ");
        }
        break;
        case 0x08:
        {
            return QString("线路#2短路");
        }
        break;
        case 0x09:
        {
            return QString("EPPROM故障");
        }
        break;
        case 0x0A:
        {
            return QString("通信故障  ");
        }
        break;
        case 0x0B:
        {
            if ((danyuan_index >= 0x01) && (danyuan_index <= 0x07))
            {
                return QString("报警灯点亮");
            }
            else if (danyuan_index == 0x0B)
            {
                return QString("报警灯点亮");
            }
            else if (danyuan_index == 0x40)
            {
                return QString("直控点启动");
            }
            else if (danyuan_index == 0x14)
            {
                return QString("确认启动  ");
            }
            else if (danyuan_index == 0x39)
            {
                return QString("  点  亮  ");
            }
            else if (danyuan_index == 0x56)
            {
                return QString("启动喷洒  ");
            }
            else
            {
                return QString("线路启动  ");
            }
        }
        break;
        case 0x0C:
        {
            return QString("线路#2启动");
        }
        break;
        case 0x0E:
        {
            return QString("调整失败  ");
        }
        break;
        case 0x0F:
        {
             return QString("通信故障");
        }
        break;

        case 0x12:
        {
            if ((danyuan_index >= 0x01) && (danyuan_index <= 0x04))
            {
                return QString("稳态值恢复");
            }
            else if ((danyuan_index >= 0x05) && (danyuan_index <= 0x06))
            {
                return QString("触点恢复  ");
            }
            else
            {
                return QString("24V恢复   ");
            }
        }
        break;
        case 0x13:
        {
            if ((danyuan_index >= 0x10) && (danyuan_index <= 0x11))
            {
                return QString("隔离恢复  ");
            }
            else
            {
                return QString("欠压恢复  ");
            }
        }
        break;
        case 0x14:
        {
            return QString("输入恢复  ");
        }
        break;
        case 0x15:
        {
            return QString("#2开路恢复");
        }
        break;
        case 0x16:
        {
            return QString("输出恢复  ");
        }
        break;
        case 0x17:
        {
            return QString("#1短路恢复");
        }
        break;
        case 0x18:
        {
            return QString("#2短路恢复");
        }
        break;
        case 0x1B:
        {
            if (danyuan_index == 0x40)
            {
                return QString("直控点停止");
            }
            else if (danyuan_index == 0x14)
            {
                return QString("确认停止  ");
            }
            else if (danyuan_index == 0x39)
            {
                return QString("  熄  灭  ");
            }
            else if (danyuan_index == 0x56)
            {
                return QString("停止喷洒  ");
            }
            else
            {
                return QString("线路停止  ");
            }
        }
        break;
        case 0x1C:
        {
            return QString("线路#2停止");
        }
        break;
        case 0x20:
        {
            if (danyuan_index == 0x3B)
            {
                return QString("转换至自动");
            }
            else
            {
                return QString("动作失败  ");
            }
        }
        break;
        case 0x2B:
        {
            if (danyuan_index == 0x3B)
            {
                return QString("转换至手动");
            }
            else
            {
                return QString("不存在  ");
            }
        }
        break;
        case 0x35:
        {
            if ((danyuan_index >= 0x09) && (danyuan_index <= 0x0A))
            {
                return QString("#1反馈撤销");
            }
            else if (danyuan_index == 0x14)
            {
                return QString("反馈撤销  ");
            }
            else
            {
                return QString("直控点撤销");
            }
        }
        break;
        case 0x3A:
        {
            return QString("#1#2撤销");
        }
        break;
        case 0x54:
        {
            return QString("烟I级火警");
        }
        break;
        case 0x55:
        {
            if ((danyuan_index >= 0x01) && (danyuan_index <= 0x04))
            {
                return QString("烟II级火警");
            }
            else if (danyuan_index == 0x05)
            {
                return QString("按钮火警  ");
            }
            else if (danyuan_index == 0x07)
            {
                return QString("输入反馈  ");
            }
            else if (danyuan_index == 0x14)
            {
                return QString("  反  馈  ");
            }
            else if (danyuan_index == 0x38)
            {
                return QString("紧急启动  ");
            }
            else if (danyuan_index == 0x3B)
            {
                return QString("转换为自动");
            }
            else if (danyuan_index == 0x56)
            {
                return QString("紧急启动  ");
            }
            else if (danyuan_index == 0x40)
            {
                return QString("直控点反馈");
            }
            else if ((danyuan_index >= 0x09) && (danyuan_index <= 0x0A))
            {
                return QString("线路#1反馈");
            }
            else
            {
                return QString("输入火警  ");
            }
        }
        break;
        case 0x56:
        {
            if (danyuan_index == 0x56)
            {
                return QString("  喷  洒  ");
            }
            else
            {
                return QString("  喷  洒  ");
            }
        }
        break;
        case 0x57:
        {
            if (danyuan_index == 0x57)
            {
                return QString("喷洒撤销  ");
            }
            else
            {
                return QString("喷洒撤销  ");
            }
        }
        break;
        case 0xA4:
        {
            return QString("温I级火警");
        }
        break;
        case 0xA5:
        {
            if (danyuan_index == 0x0A)
            {
                return QString("线路#2反馈");
            }
            else if (danyuan_index == 0x38)
            {
                return QString("紧急停止  ");
            }
            else if (danyuan_index == 0x3B)
            {
                return QString("转换为手动");
            }
            else if (danyuan_index == 0x56)
            {
                return QString("紧急停止  ");
            }
            else
            {
                return QString("温II级火警");
            }
        }
        break;
        case 0xAA:
        {
            if (danyuan_index == 0x0A)
            {
                return QString("#1#2反馈  ");
            }
            else if (danyuan_index == 0x03)
            {
                return QString("复合火警  ");
            }
            else if (danyuan_index == 0x0B)
            {
                return QString("输入火警  ");
            }
            else
            {
                return QString("输入火警  ");
            }
        }
        break;
        case 0xAC:
        {
            return QString("火警撤销  ");
        }
        break;
        case 0xB0:
        {
            return QString("  屏  蔽  ");
        }
        break;
        case 0xB1:
        {
            return QString("屏蔽恢复  ");
        }
        break;
        case 0xB2:
        {
            return QString("  火  警  ");
        }
        break;
        case 0xB3:
        {
            return QString("  故  障  ");
        }
        break;
        case 0xB4:
        {
            return QString("  联  动  ");
        }
        break;
        case 0xB8:
        {
            return QString("可撤销预警");
        }
        break;
        case 0xB9:
        {
            return QString("预警撤销  ");
        }
        break;
        case 0xBA:
        {
            return QString("可撤销火警");
        }
        break;
        case 0xBB:
        {
            return QString("火警撤销   ");
        }
        break;
        case 0x81:
        {
            return QString("  延  时  ");
        }
        break;
        case 0x82:
        {
            return QString("#1延时停止");
        }
        break;
        case 0x91:
        {
            return QString("#2延时启动");
        }
        break;
        case 0x92:
        {
            return QString("#2延时停止");
        }
        break;
        case 0x93:
        {
            return QString("延时结束  ");
        }
        break;
        case 0x94:
        {
            return QString("延时烟I级 ");
        }
        break;
        case 0x95:
        {
            if ((danyuan_index >= 0x01) && (danyuan_index <= 0x04))
            {
                return QString("延时烟II级");
            }
            else
            {
                return QString("延时火警  ");
            }
        }
        break;
        case 0x96:
        {
            return QString("延时温I级 ");
        }
        break;
        case 0x97:
        {
            return QString("延时温II级");
        }
        break;
        case 0x98:
        {
            return QString("延时复合警");
        }
        break;
        case 0xD1:
        {
            return QString("主电停止  ");
        }
        break;
        case 0xD2:
        {
            return QString("备电停止  ");
        }
        break;
        case 0xD3:
        {
            return QString("备电欠压  ");
        }
        break;
        case 0xD4:
        {
            return QString("主电欠压  ");
        }
        break;
        case 0xD5:
        {
            return QString("主电启动  ");
        }
        break;
        case 0xD6:
        {
            return QString("备电启动  ");
        }
        break;
        case 0xD7:
        {
            return QString("备电恢复  ");
        }
        break;
        case 0xD8:
        {
            return QString("主电恢复  ");
        }
        break;
        case 0xD9:
        {
            return QString("打印机故障");
        }
        break;
        case 0xDA:
        {
            return QString("打印机恢复");
        }
        break;
        case 0xDC:
        {
            return QString("自检结束  ");
        }
        break;
        case 0xDD:
        {
            return QString("系统自检  ");
        }
        break;
        case 0xDE:
        {
            return QString("系统复位  ");
        }
        break;
        case 0xDF:
        {
            return QString("系统上电  ");
        }
        break;
        case 0xE1://add by qingwu 2013.4.2
        {
            return QString("声光故障  ");
        }
        break;
        case 0xE2:
        {
            return QString("声光恢复  ");
        }
        break;
        case 0xEF:
        {
            /*add by qingwu.for link
            0EFH 001H 001H 启动未反馈
            0EFH 002H 002H 启动已反馈
            0EFH 003H 003H 联动反馈
            */
            switch(danyuan_index)
            {
            case 0x01:
                return QString("启动未反馈");
                break;
            case 0x02:
                return QString("启动已反馈");
                break;
            case 0x03:
                return QString("联动反馈");
                break;
            }
        }
        break;
        default:
        {
            return QString("不在线");
        }
        break;
    }
}
QString Type::char_danyuan(int danyuan_index)
{
    switch(danyuan_index)
    {
        case 0x00:
        {
            return QString("接口板");// 接口板 <---所有类型
        }
        break;
        case 0x01:
        {
            return QString("单烟探头");
        }
        break;
        case 0x02:
        {
            return QString("单温探头");
        }
        break;
        case 0x03:
        {
            return QString("复合探头");
        }
        break;
        case 0x05:
        {
            return QString("手报按钮");
        }
        break;
        case 0x06:
        {
            return QString("消火栓  ");
        }
        break;
        case 0x07:
        {
            return QString("输入模块");
        }
        break;
        case 0x08:
        {
            return QString("输出模块");
        }
        break;
        case 0x09:
        {
            return QString("输入输出");
        }
        break;
        case 0x0A:
        {
            return QString("双入双出");
        }
        break;
        case 0x0B:
        {
            return QString("编码接口");
        }
        break;
        case 0x0C:
        {
            return QString("红外探头");
        }
        break;
        case 0x0D:
        {
            return QString("感温电缆");
        }
        break;
        case 0x10:
        {
            return QString("两线短隔");
        }
        break;
        case 0x11:
        {
            return QString("四线短隔");
        }
        break;
        case 0x14:
        {
            return QString("声光报警");
        }
        break;
        case 0x20:
        {
            return QString("报警设备");
        }
        break;
        case 0x21:
        {
            return QString("联动设备");
        }
        break;
        case 0x38:
        {
            return QString("启动按钮");
        }
        break;
        case 0x39:
        {
            return QString("放气指示");
        }
        break;
        case 0x3A:
        {
            return QString("声光警报");
        }
        break;
        case 0x3B:
        {
            return QString("手自转换");
        }
        break;
        case 0x40:
        {
            return QString("直控点  ");
        }
        break;
        case 0x41:
        {
            return QString("  楼显  ");
        }
        break;
        case 0x42:
        {
            return QString("总线点  ");
        }
        break;
        case 0x50:
        {
            return QString("控制器  ");
        }
        break;
        case 0x51:
        {
            return QString("回  路  ");
        }
        break;
        case 0x52:
        {
            return QString("直控盘  ");
        }
        break;
        case 0x53:
        {
            return QString("总线盘  ");
        }
        break;
        case 0x54:
        {
            return QString("转接模块");
        }
        break;
        case 0x55:
        {
            return QString("气体回路");
        }
        break;
        case 0x56:
        {
            return QString("气体盘  ");
        }
        break;
        default:
        {
            return QString("不存在  ");
        }
        break;
    }
}

int Type::int_danyuanId(const QString &danyuan_string)
{
    if (danyuan_string == "所有类型")
    {
        return 0x00;
    }
    else if (danyuan_string == "单烟探头")
    {
        return 0x01;
    }
    else if (danyuan_string == "单温探头")
    {
        return 0x02;
    }
    else if (danyuan_string == "复合探头")
    {
        return 0x03;
    }
    else if (danyuan_string == "手报按钮")
    {
        return 0x05;
    }
    else if (danyuan_string == "消火栓  ")
    {
        return 0x06;
    }
    else if (danyuan_string == "输入模块")
    {
        return 0x07;
    }
    else if (danyuan_string == "输出模块")
    {
        return 0x08;
    }
    else if (danyuan_string == "输入输出")
    {
        return 0x09;
    }
    else if (danyuan_string == "双入双出")
    {
        return 0x0A;
    }
    else if (danyuan_string == "编码接口")
    {
        return 0x0B;
    }
    else if (danyuan_string == "红外探头")
    {
        return 0x0C;
    }
    else if (danyuan_string == "感温电缆")
    {
        return 0x0D;
    }
    else if (danyuan_string == "两线短隔")
    {
        return 0x10;
    }
    else if (danyuan_string == "四线短隔")
    {
        return 0x11;
    }
    else if (danyuan_string == "声光报警")
    {
        return 0x14;
    }
    else if (danyuan_string == "报警设备")
    {
        return 0x20;
    }
    else if (danyuan_string == "联动设备")
    {
        return 0x21;
    }
    else if (danyuan_string == "启动按钮")
    {
        return 0x38;
    }
    else if (danyuan_string == "放气指示")
    {
        return 0x39;
    }
    else if (danyuan_string == "声光警报")
    {
        return 0x3A;
    }
    else if (danyuan_string == "手自转换")
    {
        return 0x3B;
    }
    else if (danyuan_string == "直控点  ")
    {
        return 0x40;
    }
    else if (danyuan_string == "  楼显  ")
    {
        return 0x41;
    }
    else if (danyuan_string == "总线点  ")
    {
        return 0x42;
    }
    else if (danyuan_string == "控制器  ")
    {
        return 0x50;
    }
    else if (danyuan_string == "回  路  ")
    {
        return 0x51;
    }
    else if (danyuan_string == "直控盘  ")
    {
        return 0x52;
    }
    else if (danyuan_string == "总线盘  ")
    {
        return 0x53;
    }
    else if (danyuan_string == "转接模块")
    {
        return 0x54;
    }
    else if (danyuan_string == "气体回路")
    {
        return 0x55;
    }
    else if (danyuan_string == "气体盘  ")
    {
        return 0x56;
    }
    else
    {
        return 0xFF;
    }
}

QString Type::char_liandong(int liandong_index)
{
    switch(liandong_index)
    {
        case 0x01:
        {
            return QString("水流指示");
        }
        break;
        case 0x02:
        {
            return QString("压力开关");
        }
        break;
        case 0x03:
        {
            return QString("报警阀  ");
        }
        break;
        case 0x04:
        {
            return QString("声光警报");
        }
        break;
        case 0x05:
        {
            return QString("消防泵  ");
        }
        break;
        case 0x06:
        {
            return QString("喷淋泵  ");
        }
        break;
        case 0x07:
        {
            return QString("增压泵  ");
        }
        break;
        case 0x08:
        {
            return QString("雨淋泵  ");
        }
        break;
        case 0x09:
        {
            return QString("非消防电");
        }
        break;
        case 0x0A:
        {
            return QString("防火门  ");
        }
        break;
        case 0x0B:
        {
            return QString("排烟阀  ");
        }
        break;
        case 0x0C:
        {
            return QString("防火阀  ");
        }
        break;
        case 0x0D:
        {
            return QString("雨淋阀  ");
        }
        break;
        case 0x0E:
        {
            return QString("排烟风机");
        }
        break;
        case 0x0F:
        {
            return QString("送风机  ");
        }
        break;
        case 0x10:
        {
            return QString("消防广播");
        }
        break;
        case 0x11:
        {
            return QString("消防电话");
        }
        break;
        case 0x12:
        {
            return QString("电梯迫降");
        }
        break;
        case 0x13:
        {
            return QString("应急照明");
        }
        break;
        case 0x14:
        {
            return QString("疏散指示");
        }
        break;
        case 0x15:
        {
            return QString("警报装置");
        }
        break;
        case 0x16:
        {
            return QString("空调切断");
        }
        break;
        case 0x17:
        {
            return QString("灭火气体");
        }
        break;
        case 0x18:
        {
            return QString("卷帘门  ");
        }
        break;
        default:
        {
            return QString("不存在  ");
        }
        break;
    }
}

int Type::int_liandongId(const QString &liandong_string)
{
    if (liandong_string == "水流指示")
    {
        return 0x01;
    }
    else if (liandong_string == "压力开关")
    {
        return 0x02;
    }
    else if (liandong_string == "报警阀  ")
    {
        return 0x03;
    }
    else if (liandong_string == "声光警报")
    {
        return 0x04;
    }
    else if (liandong_string == "消防泵  ")
    {
        return 0x05;
    }
    else if (liandong_string == "喷淋泵  ")
    {
        return 0x06;
    }
    else if (liandong_string == "增压泵  ")
    {
        return 0x07;
    }
    else if (liandong_string == "雨淋泵  ")
    {
        return 0x08;
    }
    else if (liandong_string == "非消防电")
    {
        return 0x09;
    }
    else if (liandong_string == "防火门  ")
    {
        return 0x0A;
    }
    else if (liandong_string == "排烟阀  ")
    {
        return 0x0B;
    }
    else if (liandong_string == "防火阀  ")
    {
        return 0x0C;
    }
    else if (liandong_string == "雨淋阀  ")
    {
        return 0x0D;
    }
    else if (liandong_string == "排烟风机")
    {
        return 0x0E;
    }
    else if (liandong_string == "送风机  ")
    {
        return 0x0F;
    }
    else if (liandong_string == "消防广播")
    {
        return 0x10;
    }
    else if (liandong_string == "消防电话")
    {
        return 0x11;
    }
    else if (liandong_string == "电梯迫降")
    {
        return 0x12;
    }
    else if (liandong_string == "应急照明")
    {
        return 0x13;
    }
    else if (liandong_string == "疏散指示")
    {
        return 0x14;
    }
    else if (liandong_string == "警报装置")
    {
        return 0x15;
    }
    else if (liandong_string == "空调切断")
    {
        return 0x16;
    }
    else if (liandong_string == "灭火气体")
    {
        return 0x17;
    }
    else if (liandong_string == "卷帘门  ")
    {
        return 0x18;
    }
    else
    {
        return 0xFF;
    }
}
