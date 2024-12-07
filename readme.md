# avs-operator

avs光谱仪上位机程序

## USAGE

function of the avs-operator argparse

使用`avs-operator.exe --help`可以查看所有配置选项。

```
Usage: avs-operator [--help] [--version] [--output file_path] [--log log_file_path] [--measuretime number] [--integraltime number] [--integralnumber number] [--angle number] [--method 'average'|'maximum']

Optional arguments:
  -h, --help            shows help message and exits
  -v, --version         prints version information and exits
  -o, --output          set the output file path [nargs=0..1] [default: "."]
  -g, --log             set the log file path [nargs=0..1] [default: "None"]
  -t, --measuretime     set the measure times [nargs=0..1] [default: 1]
  -i, --integraltime    set the integral times [nargs=0..1] [default: 5]
  -n, --integralnumber  set the integral number [nargs=0..1] [default: 50]
  -a, --angle           angle setting for this [nargs=0..1] [default: 90]
  -m, --method          set method average or maximum [nargs=0..1] [default: "average"]
```
`--output`是传递一个相对或绝对路径，输出数据文件夹，例如`avs-operator.exe --output data`则可以把数据结果存储在与`avs-operator.exe`同级目录的`data`文件夹下。

`--log`是传递一个相对或绝对路径，输出日志，例如`avs-operator.exe --log log`则可以把日志存储在与`avs-operator.exe`同级目录的`log`文件夹下。

`--measuretime`是传递一个测量次数，例如`avs-operator.exe --measuretime 2`则可以运行同一角度下的多次测试，目前不可以多次测试不同角度，只可以通过多次调用`avs-operator.exe`进程来实现。

`--integraltime`是传递积分时间，例如`avs-operator.exe --integraltime 5`则可以把积分时间设置为`5`，从而可以通过读取积分时间来实现积分时间的计算。

`--integralnumber`是传递积分次数，例如`avs-operator.exe --integralnumber 50`。

`--angle`是传递一个测量角度，例如`avs-operator.exe --angle 90`，目前支持`int`整数类型。

`--method`是设置积分计算的方法，在其中有`maximum`和`average`两种，可以通过`avs-operator.exe --method maximum`传递。

## REQUIRE

使用本程序需要`avaspecx64.dll`，将`avs-operator.exe`与`avaspecx64.dll`放在同级目录下就可以运行。

## 最佳实践

创建一个文件夹，在任意位置，将`avs-operator.exe`与`avaspecx64.dll`存放在其中。目录结构为：
```
任意文件夹/
    avs-operator.exe
    avaspecx64.dll
    fmt.dll
```
右击文件夹空白处，点击任意命令行程序`powershell.exe`或者`cmd.exe`。运行如下命令行
```
avs-operator.exe --angle 90 --integraltime 4 --measuretime 1 --integralnumber 50
```
其含义是测量角度为90，积分时间为4，测量次数为1，积分次数为50。
