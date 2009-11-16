;--- ~sakane/gmapping-dist/my-readme.txt
2009/9/2 オンラインSLAM関係のメモ

この文書(my-readme)は，周君が2009/8/28に作成したonline SLAMが可能な
gmappingの使い方についてのメモです．

このプログラムには，carmen.0.4.6.i および， gt3.3.8  gsl1.9　が必要．
　/home/zhou/.cshrc を自分のディレクトリにコピーすればよいが，
ノートＰＣ mariner2では，~sakane/.cshrc でも動くように直してある．

[ハードウェア]，シリアルポートをUSBで接続した場合
	SICKレーザセンサ：ttyUSB0,  Pioneer本体の接続：ttyUSB1,

% cd gmapping-dist
% winset7 
でウィンドウを７個生成して，各ウィンドウにおいて，
(1) % param_server  -robot dorothy  carmen.ini
(2) % pioneer
(3) % laser
(4) % robot
(5) % bin/gfs_simplegui  -onLine   （オンライン地図生成を起動）

もしも，オフラインが必要ならば，
    % bin/gfs_simplegui  -filename  < log data file >  

のようにする．
