# DesktopDrawpadBlocker 桌面画板悬浮窗拦截器
DesktopDrawpadBlocker，Windows 软件插件，致力于剔除桌面上 希沃白板桌面悬浮窗等 杂乱无章的桌面画板悬浮窗，还您一个干净的桌面。

## 软件初衷
特别是在教学一体机上，一些白板软件的桌面悬浮窗画笔功能不能直接关闭，桌面上会出现多家画笔工具的悬浮窗。这容易导致老师混用不同的工具导致无法继续批注，这严重影响了教学效率。于是 DesktopDrawpadBlocker 应运而生，同时作为插件位于 [智绘教Inkeys](https://github.com/Alan-CRL/Inkeys) 中。

## 软件简介
DesktopDrawpadBlocker 其原理类似弹窗拦截，会自动关闭 希沃白板桌面悬浮窗 和 希沃PPT小工具 等窗口并具有高度自定义化。
**DesktopDrawpadBlocker 是作为 Windows 软件的插件程序，其并没有 UI界面，位于后台运行。**  

DDB 属于后台运行应用，默认 5000 毫秒扫描一次窗口，并关闭名单中的窗口，并且软件只需要普通用户权限。软件的配置文件和线程交互都通过根目录下的 json 文件，通过此 json 的变化指定程序的行为。高度的自定义化可以通过修改 json 文件完成，例如指定是否要关闭某程序，设定扫描间隔时间，设置程序行为等……

## 应用场景
DDB 使用 GPLv3 开源协议，可以作为您的软件的插件，您的软件可以使用 DDB 来达到拦截桌面画板悬浮窗的功能。
并大体具有两种典型模式：
- 同步模式（随宿主程序开启和关闭）
  您的软件启动后开启 DDB，并指定您软件的路径，当 DDB 检测到您的软件关闭后，也会同步关闭
  **重要** 此模式时，如果宿主程序未开启，程序将无法启动  
- 同步模式（随宿主程序关闭）
  启动 DDB 后，您的软件可能启动/未启动，当 DDB 检测到您的软件关闭后（先前检测到启动过），也会同步关闭
- 独立模式
  您的软件可以设置 DDB 开启自动启动，来达到全时段拦截的效果（宿主软件与 DDB 互不干扰）。想要关闭 DDB 则需要通过 json 的交互完成。
> 拓展：（可选）独立模式下，当宿主程序被关闭后，拦截到其他软件的窗口后，重启宿主程序

详细的 json 配置方法都会在下面讲到。

## 软件行为
程序开启后，会自动读取配置文件并开始工作。同时程序也会实时地察觉配置文件的更改，并做出调整。程序启动后会创建托盘图标，在此可以让其手动刷新配置或关闭程序等。

可选的拦截列表：  
| 名称 | 标识名 | 高级功能 | 备注 | 测试时版本 |
|---|---|---|---|---|
| 希沃白板3 桌面画笔悬浮窗 | `SeewoWhiteboard3Floating` ||| 3.1.2.3606 |
| 希沃白板5 桌面画笔悬浮窗 | `SeewoWhiteboard5Floating` ||| 5.2.4.9592 |
| 希沃轻白板（5C） 桌面画笔悬浮窗 | `SeewoWhiteboard5CFloating` ||| 1.0.1.9068 |
| 希沃品课教师端 侧栏悬浮窗 | `SeewoPincoSideBarFloating` ||| v1.2.43.7298 |
| 希沃品课教师端 桌面画笔悬浮窗（包括PPT控件） | `SeewoPincoDrawingFloating` ||| v1.2.43.7298 |
| 希沃PPT小工具 | `SeewoPPTFloating` ||| 1.0.0.795 |
| 希沃课堂助手 PPT小工具 | `SeewoIwbAssistantFloating` ||| 0.0.3.1251 |
| 欧帝白板 桌面画笔悬浮窗（包括PPT控件） | `YiouBoardFloating` || 需要DDB管理员 | 2.1.111 |
| AiClass 桌面画笔悬浮窗 | `AiClassFloating` ||| 5.19.3.2 |
| ClassIn X 桌面画笔悬浮窗 | `ClassInXFloating` ||| 6.0.5.3913 |
| 天喻教育云互动课堂 桌面画笔悬浮窗（包括PPT控件） | `IntelligentClassFloating` ||| 3.1.3 |
| 畅言智慧课堂4.0 桌面画笔悬浮窗（包括PPT控件） | `ChangYanFloating` | 自动恢复 | 需要DDB管理员 | 4.28.2 |
| 畅言智慧课堂5.0 桌面画笔悬浮窗（包括PPT控件） | `ChangYan5Floating` | 自动恢复 | 需要DDB管理员 | 5.7.0.1 |
| C30智能教学 侧栏悬浮窗 | `Iclass30SidebarFloating` || 需要DDB管理员 | 1.3.1460.0 |
| C30智能教学 桌面画笔悬浮窗（包括PPT控件） | `Iclass30Floating` | 自动恢复和窗口追踪 | 需要DDB管理员 | 1.3.1460.0 |
| 希沃桌面 侧栏悬浮窗 | `SeewoDesktopSideBarFloating` || 需要DDB管理员; 1.0/2.0/2.5/3.0 普教版/高教版 通用 | S.6.0.3.63; 1.0.0.63; 7.0.0.25; 8.1.0.49 |
| 希沃桌面 桌面画笔悬浮窗 | `SeewoDesktopDrawingFloating` || 需要DDB管理员; 1.0/2.0/2.5/3.0 普教版/高教版 通用 | S.6.0.3.63; 1.0.0.63; 7.0.0.25; 8.1.0.49 |

如果有新的拦截需求，请添加 Issues。  

## 高级功能
### 自动恢复
一些软件的桌面画笔悬浮窗和白板界面悬浮窗为同一窗口，此功能则可以在识别到处于白板界面的情况下，恢复显示悬浮窗，使白板功能正常使用。

### 窗口追踪
一些软件的窗口状态改变过于迅速，窗口追踪功能将在拦截失效的时候立即进行拦截（而不是等待至下一轮拦截），以保证体验。  
**特别的**，如果同时拥有自动恢复功能，请不要将拦截间隔设置超过 5000 毫秒，否则将出现在白板界面时需要较长时间才能使悬浮窗显示。  

## 兼容性
程序使用 C++20 编写，最低兼容 Windows 7 RTM（sp0），并且单文件运行无需额外的运行库。运行时内存占用仅约 **5MB**。

## 软件配置
软件的交互和配置都由其目录下的 `interaction_configuration.json` 完成。（您可以先启动一次 DDB，会生成使用模板）    
缺省设置中，项目的开启和关闭默认值如下所示。  

```json
{
	"Edition" : "20260205a",
	"Intercept" : 
	{
		"AiClassFloating" : true,
		"ChangYan5Floating" : true,
		"ChangYanFloating" : true,
		"ClassInXFloating" : true,
		"Iclass30Floating" : true,
		"Iclass30SidebarFloating" : false,
		"IntelligentClassFloating" : true,
		"SeewoDesktopDrawingFloating" : true,
		"SeewoDesktopSideBarFloating" : false,
		"SeewoIwbAssistantFloating" : true,
		"SeewoPPTFloating" : true,
		"SeewoPincoDrawingFloating" : true,
		"SeewoPincoSideBarFloating" : false,
		"SeewoWhiteboard3Floating" : true,
		"SeewoWhiteboard5CFloating" : true,
		"SeewoWhiteboard5Floating" : true,
		"YiouBoardFloating" : true
	},
	"Mode" : 
	{
		"HostPath" : "",
		"Mode" : 0,
		"RestartHost" : false
	},
	"SleepTime" : 5000,
	"~ConfigurationChange" : false,
	"~KeepOpen" : false
}
```

[utf8 string] `Edition`：表示 DDB 版本，由 DDB 反馈  
[bool] `~ConfigurationChange`：当配置更改时应写为 true，DDB 将更新配置  
> 启动程序时此项应为 true（开启自启时除外），当 DDB 成功读取配置后，该项将变为 false

[bool] `~KeepOpen`：决定程序是否需要开启，**手动关闭程序，将此项写为 false 即可**  
> 启动程序时此项应为 true（开启自启时除外），当程序关闭后此项将变为 false

[int32] `SleepTime`：每次扫描窗口的间隔（单位 ms）  

[int32] `Mode` `Mode`：程序模式（0 独立模式 1 随宿主程序和开启和关闭 2 随宿主程序关闭）  
[bool] `Mode` `RestartHost`：（模式为 `程序模式0` 时）是否启用拓展功能  
> 拓展：（可选）独立模式下，当宿主程序被关闭后，拦截到其他软件的窗口后，重启宿主程序（前提是宿主程序文件存在）   

[utf8 string] `Mode` `HostPath`：（utf8，模式不为 `程序模式0` 或 开启拓展功能时 **必须**）宿主程序路径  

[bool] `Intercept` `[xxx]`：上述拦截列表项，为 true 则拦截

在独立模式下，您的程序需要将程序设置成为软件启动项（或使用您的软件启动），DDB 会读取配置并开始工作。
**添加开机启动项时务必注意需要添加启动参数：** `-startup` 或在根目录下创建一个空白文件 `start_up.signal`

## 工作流程

进入程序主循环后，会按照下列顺序执行操作

1. 查询程序是否需要关闭（如果是则**立即**关闭程序）
2. 查询配置文件是否修改（如果是则更新配置）
3. 检测并拦截窗口
4. 符合条件则启用扩展功能：重启宿主程序
5. 等待 `SleepTime` 毫秒

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

1. 将配置写入 json 文件，指定 `~ConfigurationChange` 为 true，`~KeepOpen` 为 false  
（DDB 关闭成功后，`~ConfigurationChange` 和 `~KeepOpen` 变为 false）

`!` **Tips** DDB 只会在主循环 2-3 步骤时读取并反馈配置，注意不是实时反应。  
`!` **Tips** DDB 在同步模式中宿主程序是否关闭的扫描间隔是 500ms，但只会在完成一个工作流程后关闭程序（也就是说会被工作流程5阻塞一段时间再关闭，发出关闭信号后，程序将不会继续检测宿主程序状态直至关闭）

## FAQ

`Q` DDB 运行时修改配置会不会导致文件冲突？  
`A` DDB 会在 json 拥有读写权限时再读取和修改 json，修改和读取时会分别占用写权限和读权限，同时您的程序在修改 json 应至少占用写权限  

`Q` 开机启动时需要设置 `~ConfigurationChange` 和 `~KeepOpen` 值吗？  
`A` 不需要，但是**添加开机启动项时务必注意需要添加启动参数：** `-startup` 或在根目录下创建一个空白文件 `start_up.signal`，开机启动时不会检查这个两个值的合理性。 

`Q` `interaction_configuration.json` 的文件编码格式是？  
`A` Utf8 或 Utf8 BOM（带签名），程序输出的 json 和输入编码一致。   

## 项目引用
[Alan-CRL/Inkeys](https://github.com/Alan-CRL/Inkeys)  
[open-source-parsers/jsoncpp](https://github.com/open-source-parsers/jsoncpp)  
[Neargye/magic_enum](https://github.com/Neargye/magic_enum)  
[efficient/libcuckoo](https://github.com/efficient/libcuckoo)  