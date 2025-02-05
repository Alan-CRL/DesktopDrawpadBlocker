# DesktopDrawpadBlocker 桌面画板悬浮窗拦截器
DesktopDrawpadBlocker，Windows 软件插件，致力于剔除桌面上 希沃白板桌面悬浮窗等 杂乱无章的桌面画板悬浮窗，还您一个干净的桌面。

## 软件简介
DesktopDrawpadBlocker 其原理类似弹窗拦截，会自动关闭 希沃白板桌面悬浮窗 和 希沃PPT小工具 等窗口并具有高度自定义化。
**DesktopDrawpadBlocker 是作为 Windows 软件的插件程序，其并没有 UI界面，位于后台运行。**  

DDB 属于后台运行应用，默认 5000 毫秒扫描一次窗口，并关闭名单中的窗口，并且软件只需要普通用户权限。软件的配置文件和线程交互都通过根目录下的 json 文件，通过此 json 的变化指定程序的行为。高度的自定义化可以通过修改 json 文件完成，例如指定是否要关闭某程序，设定扫描间隔时间，设置程序行为等……

## 应用场景
DDB 使用 GPLv3 开源协议，可以作为您的软件的插件，您的软件可以使用 DDB 来达到拦截桌面画板悬浮窗的功能。
并大体具有两种典型模式：
- 同步模式（随宿主程序开启和关闭/随宿主程序关闭）
  您的软件启动后开启 DDB，并指定您软件的路径，当 DDB 检测到您的软件关闭后，也会同步关闭
- 独立模式
  您的软件可以设置 DDB 开启自动启动，来达到全时段拦截的效果（宿主软件与 DDB 互不干扰）
> 拓展：（可选）独立模式下，当宿主程序被关闭后，拦截到其他软件的窗口后，重启宿主程序

详细的 json 配置方法都会在下面讲到。

## 软件行为
程序开启后，会自动读取配置文件并开始工作。同时程序也会实时地察觉配置文件的更改，并做出调整。程序启动后会创建托盘图标，在此可以让其手动刷新配置或关闭程序等。

可选的拦截列表：  
| 名称 | 标识名 | 备注 |
|---|---|---|
| 希沃白板3 桌面悬浮窗 | `SeewoWhiteboard3Floating` ||
| 希沃白板5 桌面悬浮窗 | `SeewoWhiteboard5Floating` ||
| 希沃白板5C 桌面悬浮窗 | `SeewoWhiteboard5CFloating` ||
| 希沃品课桌面悬浮窗（包括PPT控件） | `SeewoPincoFloating` ||
| 希沃PPT小工具 | `SeewoPPTFloating` ||
| AiClass 桌面悬浮窗 | `AiClassFloating` ||
| 鸿合屏幕书写 | `HiteAnnotationFloating` ||
| 畅言智慧课堂（包括PPT控件） | `ChangYanFloating` | 需要管理员权限 |
| 天喻教育云互动课堂（包括PPT控件） | `IntelligentClassFloating` ||

如果有新的拦截需求，请添加 Issues

## 兼容性
程序使用 C++17 编写，最低兼容 Windows 7 sp0，并且单文件运行无需额外的运行库。

## 软件配置
软件的交互和配置都由其目录下的 `interaction_configuration.json` 完成。（您可以先启动一次 DDB，会生成使用模板）

```json
{
	"Edition" : "20250205a",
	"Intercept" : 
	{
		"AiClassFloating" : true,
		"ChangYanFloating" : true,
		"HiteAnnotationFloating" : true,
		"IntelligentClassFloating" : true,
		"SeewoPPTFloating" : true,
		"SeewoPincoFloating" : true,
		"SeewoWhiteboard3Floating" : true,
		"SeewoWhiteboard5CFloating" : true,
		"SeewoWhiteboard5Floating" : true
	},
	"Mode" : 
	{
		"HostPath" : "",
		"Mode" : 0,
		"RestartHost" : true
	},
	"SleepTime" : 5000,
	"~ConfigurationChange" : false,
	"~KeepOpen" : false
}
```

[utf8 string] `Edition`：表示 DDB 版本，由 DDB 反馈  
[bool] `~ConfigurationChange`：当配置更改时应为 true，DDB 将更新配置  
> 启动程序时此项因为 true（开启自启时除外），当 DDB 成功读取配置后，该项将变为 false

[bool] `~KeepOpen`：决定程序是否需要开启  
> 启动程序时此项因为 true（开启自启时除外），当程序关闭后此项将变为 false

[int32] `SleepTime`：每次扫描窗口的间隔（单位 ms）  

[int32] `Mode` `Mode`：程序模式（0 独立模式 1 随宿主程序和开启和关闭 2 随宿主程序关闭）  
[bool] `Mode` `RestartHost`：（模式为 0 时 **必须**）启用上述拓展功能  
[utf8 string] `Mode` `HostPath`：（utf8，模式不为 0 或 开启拓展功能时 **必须**）宿主程序路径  

[bool] `Intercept` `[xxx]`：上述拦截列表项，为 true 则拦截

在独立模式下，您的程序需要将程序设置成为软件启动项（或使用您的软件启动），DDB 会读取配置并开始工作。
**添加开机启动项时务必注意需要添加启动参数：**` -startup` 或在根目录下创建一个空白文件 `start_up.signal`

## 工作流程

进入程序主循环后，会按照下列顺序执行操作

1. 等待文件占用状态解除
2. 查询配置文件是否修改（如果是则更新配置）
3. 查询程序是否需要关闭
4. 拦截窗口
5. 扩展功能：重启宿主程序
6. 等待 `SleepTime` 毫秒

## 操控程序

按照以下步骤启动 DDB：

1. 将配置写入 json 文件，指定 `~ConfigurationChange` 和 `~KeepOpen` 为 true
2. 启动 DDB 的 exe 文件  
（程序成功启动后，`~ConfigurationChange` 变为 false 和 `~KeepOpen` 保持为 true）
3. DDB 读取配置后开始工作

按照以下步骤修改 DDB 配置：

1. 将修改后的配置写入 json 文件，指定 `~ConfigurationChange` 和 `~KeepOpen` 为 true  
（读取配置成功后，`~ConfigurationChange` 变为 false 和 `~KeepOpen` 保持为 true）

按照以下步骤关闭 DDB：

1. 将配置写入 json 文，指定 `~ConfigurationChange` 为 true，`~KeepOpen` 为 false  
（DDB 关闭成功后，`~ConfigurationChange` 和 `~KeepOpen` 变为 false）

`!` **Tips** DDB 只会在主循环 2-3 步骤时读取并反馈配置，注意不是实时反应。

## FAQ

`Q` DDB 运行时修改配置会不会导致文件冲突？  
`A` DDB 会在 json 拥有读写权限时再读取和修改 json，同时您的程序在修改 json 应至少占用写权限  

`Q` 开启启动时需要设置 `~ConfigurationChange` 和 `~KeepOpen` 值吗？  
`A` 不需要，但是**添加开机启动项时务必注意需要添加启动参数：**` -startup` 或在根目录下创建一个空白文件 `start_up.signal`，开机启动时不会检查这个两个值的合理性。 

`Q` `interaction_configuration.json` 的文件编码格式是？
`A` Utf8 或 Utf8 BOM（带签名），程序输出的 json 和输入编码一致。   

## 项目引用
[智绘教Inkeys](https://github.com/Alan-CRL/Intelligent-Drawing-Teaching)  
[JsonCpp](https://github.com/open-source-parsers/jsoncpp)  
