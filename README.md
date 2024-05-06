# DesktopDrawpadBlocker
DesktopDrawpadBlocker，Windows 软件插件，致力于剔除桌面上 希沃白板桌面悬浮窗等 杂乱无章的桌面画板悬浮窗，还您一个干净的桌面。

## 软件简介
DesktopDrawpadBlocker 其原理类似弹窗拦截，会自动关闭 希沃白板桌面悬浮窗 和 希沃PPT小工具 等窗口并具有高度自定义化。
**DesktopDrawpadBlocker 是作为 Windows 软件的插件程序，其并没有 UI界面，位于后台运行。**  

DDB 属于后台运行应用，默认 5 秒扫描一次窗口，并关闭名单中的窗口，并且软件只需要普通用户权限。软件的配置文件和线程交互都处于其跟目录下的 json 文件中，通过此 json 的变化指定程序的行为。高度的自定义化可以通过修改 json 文件完成，例如指定是否要关闭某程序，设定扫描间隔时间，设置程序行为等……

## 应用场景
DDB 使用 GPLv3 开源协议，可以作为您的软件的插件，您的软件可以使用 DDB 来达到拦截桌面画板悬浮窗的功能。
并大体具有两种典型模式：
- 同步模式
  您的软件启动后开启 DDB，并指定您软件的路径，当 DDB 检测到您的软件关闭后，也会同步关闭
- 增强模式
  您的软件可以设置 DDB 开启自动启动，来达到全时段拦截的效果
详细的 json 配置方法都会在下面降到

## 软件行为
程序开启后，会自动读取配置文件并开始工作。同时程序也会实时地察觉配置文件的更改，并做出调整。程序启动后会创建托盘图标，在此可以让其手动刷新配置或关闭程序等。

## 兼容性
程序使用 C++17 编写，最低兼容 Win7 sp0，并且单文件运行无需额外的运行库。

## 项目引用
[智绘教Inkeys](https://github.com/Alan-CRL/Intelligent-Drawing-Teaching)
[JsonCpp](https://github.com/open-source-parsers/jsoncpp)
[Spdlog](https://github.com/gabime/spdlog)
