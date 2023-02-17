# YGODrawSimulation
ygopro的卡组编辑器&amp;抽卡模拟器

## 零、动机

-	合理化卡牌构筑。最近迷上游戏王打牌，但是乱组卡组，常常搞60张经常卡手，希望能有抽卡模拟器计算抽卡上手概率以帮助卡组构筑。
-	学习一门桌面程序开发语言。工作经常需要写一些桌面小工具提高效率，故借此机会学习QT。
-	提高编程素养。害，经常项目开始规划的好好的，到后期就写的越来越乱最终变成屎山，还是编程素养不够。~~这个项目最后也成了屎山。~~

## 一、使用工具及语言
-	使用Qt6.3.1，主要用在了几个不重要的地方，稍加修改可使用Qt旧版本。因为Qt和C++的跨平台特性，本程序可以在多平台运行。
-	配置文件使用C++17标准（实际C++11），MSVC2019。


## 二、界面

![](https://github.com/Ventery/YGODrawSimulation/blob/master/ExamplePic/%E7%95%8C%E9%9D%A21.jpg)
![](https://github.com/Ventery/YGODrawSimulation/blob/master/ExamplePic/%E7%95%8C%E9%9D%A22.jpg)

## 三、安装方式
-	0、使用qt编译源码。
-	1、在执行文件目录创建目录ygoData。
-	2、将源码文件夹的/ExamplePic/DefaultSide.jpg图片文件复制到执行目录ygoData下。
-	3、将ygopro发行版本的cdb和picture文件夹复制到ygoData目录中。此时执行文件目录和ygoData目录看起来是这样的
![](https://github.com/Ventery/YGODrawSimulation/blob/master/ExamplePic/%E6%89%A7%E8%A1%8C%E6%96%87%E4%BB%B6%E7%9B%AE%E5%BD%95.jpg)
![](https://github.com/Ventery/YGODrawSimulation/blob/master/ExamplePic/ygoData%E7%9B%AE%E5%BD%95.jpg)
-	4、在Qt creator中执行，或者打包后执行。
    
## 四、操作方式
-	操作并不复杂，左键选中操作的卡组，通过右键添加或者删除卡片，点击按钮可以保存或者重置修改的卡组。
-	计算概率的界面，这些备选组合之间的关系是“或”的关系。如图所示，计算的41.2290%概率为左侧主卡组，起手抽到<font color= "#dd0000">圆武</font> 或者 <font color= "#dd0000">挖矿+剑指</font> 或者 <font color= "#dd0000">挖矿+墓指</font>  或者 <font color= "#dd0000">小世界+墓指</font> 或者 <font color= "#dd0000">挖矿+剑指</font>五种组合的概率。代表起手能展开且有护航防灰的概率。
![](https://github.com/Ventery/YGODrawSimulation/blob/master/ExamplePic/%E7%95%8C%E9%9D%A22.jpg)
-	概率计算其实是很复杂的，这里用抽卡100000次来模拟概率计算，对大概率事件误差比较小，对小概率事件误差比较大（例如起手五张尸块）。

## 五、结语
	靠斩机这套卡组在雷精上线前上了Master Duel的钻一，我太强了。
![](https://github.com/Ventery/YGODrawSimulation/blob/master/ExamplePic/%E6%8E%92%E4%BD%8D%E8%B5%9B.jpg)
