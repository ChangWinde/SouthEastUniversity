%%
%% This is file `seuthesis.cls',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% seuthesis.dtx  (with options: `cls')
%% 
%% Copyright (C) 2007-2012 by Xu Yuan <xuyuan.cn@gmail.com>
%% 
%% This file may be distributed and/or modified under the
%% conditions of the LaTeX Project Public License, either version 1.3a
%% of this license or (at your option) any later version.
%% The latest version of this license is in:
%% 
%% http://www.latex-project.org/lppl.txt
%% 
%% and version 1.3a or later is part of all distributions of LaTeX
%% version 2004/10/01 or later.
%% 
%% To produce the documentation run the original source files ending with `.dtx'
%% through LaTeX.
%% 
 \NeedsTeXFormat{LaTeX2e}[1999/12/01]
 \ProvidesClass{seuthesis}[2012/04/19 2.1.2 The LaTeX template for thesis of Southeast University]
 \typeout{Document Class `seuthesis' v2.1.2 by Xu Yuan, Song Yihan, Huang Xiaoyu (2012/04/19)}
\newif\ifbachelordegree\bachelordegreefalse
\newif\ifmasterdegree\masterdegreetrue
\newif\ifengineering\engineeringfalse
\DeclareOption{bachelor}{\bachelordegreetrue}
\DeclareOption{master}{\masterdegreetrue}
\DeclareOption{engineering}{\engineeringtrue\masterdegreetrue}
\DeclareOption{doctor}{\masterdegreefalse}
\newif\ifnocolorlinks\nocolorlinksfalse
\DeclareOption{nocolorlinks}{\nocolorlinkstrue}
\newif\ifdoublecover\doublecoverfalse
\DeclareOption{doublecover}{\doublecovertrue}
\DeclareOption{printedition}{\doublecovertrue\nocolorlinkstrue}
\newif\ifstoneside\stonesidefalse
\DeclareOption{printoneside}{%
  \PassOptionsToClass{openany}{scrbook}
  \stonesidetrue
}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{scrbook}}
\ProcessOptions
%\LoadClass[a4paper,captions=tableheading]{scrbook}
\LoadClass[a4paper,10.5pt,tablecaptionabove]{scrbook}
\let\todayeng\today

% my change
\RequirePackage{lmodern}
\RequirePackage{doc}
\RequirePackage{keyval}
\RequirePackage{ifthen}
\RequirePackage{xltxtra}
\RequirePackage{graphicx}
\RequirePackage[dvipsnames]{xcolor}
\RequirePackage{indentfirst}
\RequirePackage{makeidx} % 索引
\RequirePackage[nottoc]{tocbibind} % 将参考文献、索引、表格目录和图形
\RequirePackage[numbers,sort&compress]{natbib} % 修改引用参考文献格式
\RequirePackage{titletoc}
\RequirePackage{array} % 扩展了 array 和 tabular 环境功能
\RequirePackage{caption} % 定义图的标题格式：居中. 使用caption3.0
\captionsetup{%
  format=plain,%
  labelformat=simple,%
  justification=centering,%
  singlelinecheck=off}
\RequirePackage{paralist}% 修改列表格式
\setdefaultenum{1)}{a)}{i.}{A.}% 设置列表模式
\setdefaultleftmargin{4ex}{}{}{}{}{}
\RequirePackage[CJKnumber,slantfont,boldfont]{xeCJK}
\RequirePackage{CJKulem}

% my change
\RequirePackage{ulem}

% 加粗改黑体
% \renewcommand{\textbf}[1]{{\normalfont\bfseries #1}}
% my change
%\def\CJK@spaceChar{\hskip \f@size \p@}
\def\CJKindent{%
    %\settowidth\CJK@twochars{\CJK@spaceChar\CJK@spaceChar}
    % \parindent\CJK@twochars
    \setlength{\parindent}{2em}
    }


% 修改中文字体族，增加黑体
\setCJKmainfont[
Path = ./font/,
BoldFont=simhei.ttf,
ItalicFont=simkai.ttf,
BoldItalicFont=simfang.ttf
]{simsun.ttf}
\setCJKfamilyfont{zhsong}[Path = ./font/]{simsun.ttf}
\newcommand{\song}{\CJKfamily{zhsong}}
\setCJKfamilyfont{zhhei}[Path = ./font/]{simhei.ttf}
\newcommand{\hei}{\CJKfamily{zhhei}}
\setCJKfamilyfont{zhkai}[Path = ./font/]{simkai.ttf}
\newcommand{\kai}{\CJKfamily{zhkai}}
\setCJKfamilyfont{zhfs}[Path = ./font/]{simfang.ttf}
\newcommand{\fs}{\CJKfamily{zhfs}}

% my change, 英文字体使用 Times New Roman
\setmainfont[
Path = ./font/,
BoldFont=timesbd.ttf,
ItalicFont=timesi.ttf,
BoldItalicFont=timesbi.ttf
]{times.ttf}




\newcommand{\chuhao}{\fontsize{42pt}{46pt}\selectfont}
\newcommand{\xiaochuhao}{\fontsize{36pt}{40pt}\selectfont}
\newcommand{\yichu}{\fontsize{32pt}{36pt}\selectfont}
\newcommand{\yihao}{\fontsize{28pt}{32pt}\selectfont}
\newcommand{\erhao}{\fontsize{21pt}{24pt}\selectfont}
\newcommand{\xiaoerhao}{\fontsize{18pt}{20}\selectfont}
\newcommand{\sanhao}{\fontsize{15.75pt}{15.75pt}\selectfont}
\newcommand{\sihao}{\fontsize{14pt}{16pt}\selectfont}
\newcommand{\xiaosihao}{\fontsize{12pt}{14pt}\selectfont}
\newcommand{\wuhao}{\fontsize{10.5pt}{13pt}\selectfont}
\newcommand{\xiaowuhao}{\fontsize{9pt}{11pt}\selectfont}
\newcommand{\liuhao}{\fontsize{7.5pt}{9pt}\selectfont}
\newcommand{\xiaoliuhao}{\fontsize{6.5pt}{7.5pt}\selectfont}
\newcommand{\qihao}{\fontsize{5.5pt}{6.5pt}\selectfont}


  \renewcommand\frontmatter{%
  \ifstoneside\clearpage\else\cleardoublepage\fi

  \@mainmatterfalse
  \pagenumbering{Roman}
  \pagestyle{empty}}
\renewcommand\mainmatter{%
  \ifstoneside\clearpage\else\cleardoublepage\fi
  \@mainmattertrue
  \pagenumbering{arabic}
  \pagestyle{plain}}
\renewcommand\backmatter{%
  \ifstoneside\clearpage\else\cleardoublepage\fi
  \@mainmattertrue}

% my change
\DeclareCaptionFont{capFont}{\song\wuhao} % 表格名及图名用5号宋体
\DeclareCaptionLabelSeparator{twospace}{~~}
\captionsetup{
  labelsep=twospace,% 去掉图标签后的冒号
  font={capFont},%
  figurename=图,%
  tablename=表,%
  listfigurename=插图目录,%
  listtablename=表格目录}

\RequirePackage{hyperref}
\AtBeginDocument{\CJKindent{}%
    \InputIfFileExists{seuthesis-utf8.cfg}% 引入配置文件
    {\typeout{[seuthesis]: Load seuthesis-utf8.cfg successfully!}}%
    {\typeout{[seuthesis]: Load seuthesis-utf8.cfg failed!}}%
    \makeindex%
    \wuhao%
    \linespacing{\mainlineskip}
  }
\AtEndDocument{\backcover}

\RequirePackage[top=2cm,bottom=2cm,left=2cm,right=2cm]{geometry}
\RequirePackage[pagestyles, rm]{titlesec}
\RequirePackage{titletoc} % 设置目录
\RequirePackage{titlesec}
\hypersetup{bookmarksnumbered=true}%书签中章节编号
\defaultfontfeatures{Mapping=tex-text} % --- as em-dash

% \renewcommand{\contentsname}{\textbf{目\quad \quad 录}}


\setlength{\parskip}{0pt}% 段距
\newcommand{\mainlineskip}{1.5}% 主行距1.3
\titlespacing{\section}{0pt}{0pt}{0pt} % 标题间距
\titlespacing{\subsection}{0pt}{0pt}{0pt}
\titlespacing{\subsubsection}{0pt}{1.5ex plus .05ex minus .01ex}{1ex plus .01ex}
\titlespacing{\chapter}{0pt}{0pt}{0pt}
% \titleformat{\chapter}{\bfseries}{}{}
% \titleformat*{\chapter}{\bfseries\center\sanhao\hei}
% \titleformat*{\subsection}{\xiaosihao\song}

%my change


\newcommand{\linespacing}[1]{\linespread{#1}\selectfont}% 行距命令
\newcommand{\stcleardp}{%
  \ifstoneside
  \clearpage
  \else
  \cleardoublepage
  \fi
}
\ifnocolorlinks
\hypersetup{colorlinks=false}
\else
\hypersetup{colorlinks=true}
\fi
\hypersetup{
  linkcolor=blue,
  anchorcolor=black,
  citecolor=olive,
  filecolor=magenta,
  menucolor=red,
  urlcolor=magenta
}
\newpagestyle{seustyle}{
  \sethead[][{\song{}\xiaowuhao{}东南大学本科生毕业设计（论文）}][] % 偶数页
  {}{{\song{}\xiaowuhao{}东南大学本科生毕业设计（论文）}}{} % 奇数页
  \setfoot{}{\xiaowuhao{}\thepage}{}
  \headrule
}
\newpagestyle{seufrontstyle}{
  \pagestyle{seunolabelstyle}
}
\newpagestyle{seunolabelstyle}{
  % \sethead[][{\song{}\xiaowuhao{}东南大学2020届本科生毕业设计（论文）}][]
  % {}{{\song{}\xiaowuhao{}东南大学2020届本科生毕业设计（论文）}}{}
  % \pagenumbering{Roman}
  % \setfoot{}{\thepage}{}
  % \headrule
}
\renewpagestyle{plain}{
  \pagestyle{seunolabelstyle}
}
\renewcommand{\author}[2]{
  \def\@author{#1}
  \newcommand{\@authoreng}{#2}
}
\newcommand{\@titleeng}{}
\newcommand{\@subtitleeng}{}
\renewcommand{\title}[4]{
  \def\@title{#1}
  \renewcommand{\@subtitle}{#2}
  \renewcommand{\@titleeng}{#3}
  \renewcommand{\@subtitleeng}{#4}
}
\newcommand{\newtitleline}{\hfill\\%
  \hspace*{\fill}\hspace{0em}}
\newcommand{\@advisorname}{}
\newcommand{\@advisortitle}{}
\newcommand{\@advisornameeng}{}
\newcommand{\@advisortitleeng}{}
\newcommand{\advisor}[4]{
  \renewcommand{\@advisorname}{#1}
  \renewcommand{\@advisortitle}{#2}
  \renewcommand{\@advisornameeng}{#3}
  \renewcommand{\@advisortitleeng}{#4}
}
\newif\ifbcoadvisor\bcoadvisorfalse
\newcommand{\@coadvisorname}{}
\newcommand{\@coadvisortitle}{}
\newcommand{\@coadvisornameeng}{}
\newcommand{\@coadvisortitleeng}{}
\newcommand{\coadvisor}[4]{
  \bcoadvisortrue
  \renewcommand{\@coadvisorname}{#1}
  \renewcommand{\@coadvisortitle}{#2}
  \renewcommand{\@coadvisornameeng}{#3}
  \renewcommand{\@coadvisortitleeng}{#4}
}
\newcommand{\@categorynumber}{}
\newcommand{\categorynumber}[1]{\renewcommand{\@categorynumber}{#1}}
\newcommand{\@secretlevel}{}
\newcommand{\secretlevel}[1]{\renewcommand{\@secretlevel}{#1}}
\newcommand{\@UDC}{}
\newcommand{\UDC}[1]{\renewcommand{\@UDC}{#1}}
\newcommand{\@studentid}{}
\newcommand{\studentid}[1]{\renewcommand{\@studentid}{#1}}
\newcommand{\degree}[1]{
  \renewcommand{\@appdegree}{#1}
}
\newcommand{\@majorlen}{8em}
\newcommand{\@major}{}
\newcommand{\major}[2][8em]{
  \renewcommand{\@majorlen}{#1}
  \renewcommand{\@major}{#2}
}
\newcommand{\@department}{}
\newcommand{\@departmenteng}{~}
\newcommand{\department}[2]{
  \renewcommand{\@department}{#1}
  \renewcommand{\@departmenteng}{#2}
}
\newcommand{\@submajor}{}
\newcommand{\submajor}[1]{
  \renewcommand{\@submajor}{#1}
}
\newcommand{\@defenddate}{}
\newcommand{\defenddate}[1]{\renewcommand{\@defenddate}{#1}}
\newcommand{\authorizeorganization}[1]{
  \renewcommand{\@authorizeorganization}{#1}
}
\newcommand{\@authorizedate}{}
\newcommand{\authorizedate}[1]{\renewcommand{\@authorizedate}{#1}}
\newcommand{\@committeechair}{}
\newcommand{\committeechair}[1]{\renewcommand{\@committeechair}{#1}}
\newcommand{\@readerA}{}
\newcommand{\@readerB}{}
\newcommand{\reader}[2]{
  \renewcommand{\@readerA}{#1}
  \renewcommand{\@readerB}{#2}
}
\newcommand{\@duration}{}
\newcommand{\duration}[1]{\renewcommand{\@duration}{#1}}
\newcommand{\@address}{}
\newcommand{\address}[1]{\renewcommand{\@address}{#1}}
\newif\ifseuthesisthanks\seuthesisthanksfalse
\newcommand{\seuthesis@thanks}{}
\renewcommand{\thanks}[1]{\seuthesisthankstrue\renewcommand{\seuthesis@thanks}{#1}}
\renewcommand{\maketitle}{
  %
  % 设置书签
  % Note: The `CJKbookmarks' option should only be used for bookmarks
  % *not* in Unicode.
  \GetFileInfo{seuthesis.cls} % get the correct fileversion
  \hypersetup{
    pdfauthor={\@author},
    pdftitle={\universityname{}\@academicdegree}, %文件标题
    pdfsubject={\@title \@subtitle}, %文件主题
    pdfcreator={The \seuthesis{} class (v \fileversion)} % 应用程序
  }

  \pagestyle{empty}
  \ifdoublecover
  \stcleardp
  \else\fi
  \chinesecover

  \renewcommand*\frontmatter{%
    \if@twoside\cleardoubleoddpage\else\clearpage\fi
    \@mainmatterfalse\pagenumbering{Roman}% 大写罗马字母
  }

  \ifbachelordegree
  \ifstoneside\clearpage\else\cleardoublepage\fi
  \seudeclare
  \frontmatter
  \else
  \ifdoublecover
  \stcleardp
  \else\fi
  \englishcover % 英文封面
  \thispagestyle{empty}
  \seudeclare % 独创性和使用授权声明
  \frontmatter % 开始正文之前的部分
  \pagestyle{seufrontstyle}\clearpage
  \fi
}
\newcommand{\seuthesis}{%
  S\hspace{-0.3ex}\raisebox{-0.5ex}{E}\hspace{-0.3ex}U\hspace{0.1em}%
  \textsc{Thesis}
}
\let\orig@emph=\emph
\renewcommand{\emph}[1]{{\normalfont\slshape #1}}

\newcommand{\CoverTopOption}[3][s]{
  \makebox[1.5cm][#1]{\xiaowuhao #2}:{
    \underline{\makebox[1.5cm][c]{\xiaowuhao #3}}}
}

\newcommand{\CoverBottomOption}[3][4.5cm]{
  \makebox[7cm][s]{
    {\song\xiaosihao#2} %宋体小四号
    \underline{\makebox[#1][c]{\hei\xiaosihao#3}} %黑体小四号粗体
  }
}

\newcommand{\chinesecover}{
  \ifengineering
  \def\@degreepic{engineering}
  \else
  \ifmasterdegree
  \def\@degreepic{master}
  \else
  \def\@degreepic{doctor}
  \fi
  \fi

  \begin{titlepage}
    \linespacing{1.0}
    \begin{picture}(0,0)(24,250)
      {\includegraphics[width=\textwidth,bb=0 0 1683 986]{figures/front-cover.jpg}}
    \end{picture}
    \ifengineering
    \begin{picture}(0,0)(-70,255)
      {\includegraphics[height=30pt,bb=0 0 484 55]{figures/engineering.png}}
    \end{picture}
    \else
    \begin{picture}(0,0)(-86,255)
      {\includegraphics[height=40pt,bb=0 0 1149 150]{figures/{\@degreepic}.png}}
    \end{picture}
    \fi
    \begin{picture}(0,0)(-350,50)
      {\includegraphics[width=50pt,bb=0 0 500 501]{figures/seu-color-logo.png}}
    \end{picture}

    \begin{tabular}{l}
      \CoverTopOption{\schoolcodepre}{\schoolcode} \\
      \CoverTopOption{\categorynumberpre}{\@categorynumber} \\
      \CoverTopOption{\secretlevelpre}{\@secretlevel}\\
      \CoverTopOption{U D C}{\@UDC} \\
      \CoverTopOption{\studentidpre}{\@studentid}
    \end{tabular}

    \vspace*{7cm}

    \begin{center}
      \linespacing{1.25}
      \yihao\hei\@title\\%题名 黑体一号居中
      \yihao\hei\@subtitle%副题名 黑体一号居中
    \end{center}

    \vspace*{20pt}

    \renewcommand{\arraystretch}{2.0}
    \setlength{\tabcolsep}{0pt}
    \begin{center}
      \begin{tabular}{cr}
        \makebox[3.5cm][s]{\xiaoerhao\song\authorpre} &%宋体小二
        \underline{\makebox[6cm][s]{
            \hspace*{1cm}
            \makebox[2.5cm][c]{\xiaoerhao\hei\@author}
            \makebox[2cm][r]{}}} \\%黑体小二粗体
        \makebox[3.5cm][s]{\xiaoerhao\song\advisorpre} &
        \underline{\makebox[6cm][s]{
            \hspace*{1cm}
            \makebox[2.5cm][c]{\xiaoerhao\hei\@advisorname}
            \makebox[2cm][r]{\xiaoerhao\hei\@advisortitle}}}
        \\
        &
        \ifbcoadvisor
        \underline{\makebox[6cm][s]{
            \hspace*{1cm}
            \makebox[2.5cm][c]{\xiaoerhao\hei\@coadvisorname}
            \makebox[2cm][r]{\xiaoerhao\hei\@coadvisortitle}}}
        \else
        \fi \\
      \end{tabular}
    \end{center}

    \vspace*{10pt}

    \setlength{\tabcolsep}{10pt}
    \begin{center}
      \begin{tabular}{ll}
        \CoverBottomOption{\appdegreepre}{\@appdegree} &
        \CoverBottomOption{\authorizeorganizationpre}{\@authorizeorganization} \\
        \CoverBottomOption{\majorpre}{\@major} &
        \CoverBottomOption{\defenddatepre}{\@defenddate}\\
        \CoverBottomOption{\submajorpre}{\@submajor} &
        \CoverBottomOption{\authorizedatepre}{\@authorizedate}\\
        \CoverBottomOption[4cm]{\committeechairpre}{\@committeechair} &
        \CoverBottomOption{\readerpre}{\@readerA}\\
        & \CoverBottomOption{\makebox[2.55cm]{}}{\@readerB}
      \end{tabular}
    \end{center}

    \vfill

    {\song\sihao\centerline{\today} \par}

  \end{titlepage}
  \stcleardp

  % 中文封二
  \thispagestyle{empty}%
  \begin{center}
    \xiaochuhao
    \includegraphics[height=0.63in]{figures/seu-text-logo}

    \ifmasterdegree
    \includegraphics{figures/master-hwzs}
    \else
    \includegraphics{figures/doctor-hwzs}
    \fi

    \vspace{31.5pt}

    \linespacing{1.25}
    \yihao\hei\@title\\%题名 黑体一号居中
    \yihao\hei\@subtitle%副题名 黑体一号居中

    \vspace{42pt}
    \begin{tabular}{cr}
      \makebox[3.5cm][s]{\xiaoerhao\hei{}专~~业~~名~~称：} &
      \underline{\makebox[6cm][s]{
          \hspace*{1cm}
          \makebox[2.5cm][c]{\xiaoerhao\song\textbf{\@major}}
          \makebox[2cm][r]{}}} \\
      \\
        \makebox[3.5cm][s]{\xiaoerhao\hei\authorpre} &
        \underline{\makebox[6cm][s]{
            \hspace*{1cm}
            \makebox[2.5cm][c]{\xiaoerhao\song\textbf{\@author}}
            \makebox[2cm][r]{}}} \\
        \\
        \makebox[3.5cm][s]{\xiaoerhao\hei\advisorpre} &
        \underline{\makebox[6cm][s]{
            \hspace*{1cm}
            \makebox[2.5cm][c]{\xiaoerhao\song\textbf{\@advisorname}}
            \makebox[2cm][r]{\xiaoerhao\song\textbf{\@advisortitle}}}}
        \\
        &
        \ifbcoadvisor
        \underline{\makebox[6cm][s]{
            \hspace*{1cm}
            \makebox[2.5cm][c]{\xiaoerhao\song\textbf{\@coadvisorname}}
            \makebox[2cm][r]{\xiaoerhao\song\textbf{\@coadvisortitle}}}}
        \else
        \fi \\
      \end{tabular}
  \end{center}

  \def\blfootnote{\xdef\@thefnmark{}\@footnotetext}
  \ifseuthesisthanks
  \blfootnote{\wuhao\seuthesis@thanks}
  \fi

  \stcleardp
}

\ifbachelordegree % 本科论文封面
\renewcommand{\chinesecover}{
  \begin{titlepage}
    \linespacing{1.0}
    \begin{center}
      \vspace*{20pt}
      \includegraphics[height=2cm]{figures/seu-text-logo} \\
      \vspace*{60pt}
      \yihao\hei \bachelortitle \\
      \vspace*{60pt}
      %{\xiaoerhao\song \titlepre}
      % {\erhao\song \parbox[t]{18em}{\fs\uline{\hfill\@title\hfill}}} \\
      
      % my change
      {\xiaoerhao\song\titlepre}
      {\erhao\song \parbox[t]{16em}{\fs\uline{\hfill \@title \hfill}}}\\
    

      \vspace*{40pt}
      \renewcommand{\arraystretch}{1.5}
      \setlength{\tabcolsep}{0pt}
      \begin{tabular}{clcl}
        \sanhao\fs
        \underline{\makebox[9.5em][s]{\makebox[9.5em][c]{\@department}}} &
        \sanhao
        \departmentpre &
        \sanhao\fs
        \underline{\makebox[9.5em][s]{\makebox[9.5em][c]{\@major}}} &
        \sanhao \majorpre \\
      \end{tabular}

      \vspace*{70pt}

      \renewcommand{\arraystretch}{1.1}
      \setlength{\tabcolsep}{0pt}
      \begin{tabular}{c}
        \sanhao\studentidpre\quad\underline{%
          \makebox[20em][s]{\makebox[20em][c]{%
              \textsf{\@studentid}}}} \\
        \sanhao\studentnamepre\quad\underline{\makebox[20em][s]{%
            \makebox[20em][c]{\fs{\@author}}}} \\
        \sanhao\advisorpre\quad\underline{\makebox[20em][s]{%
            \makebox[20em][c]{\fs{\@advisorname}}}}%
        \\
        \ifbcoadvisor
        \sanhao\coadvisorpre\quad\underline{\makebox[20em][s]{%
            \makebox[20em][c]{\fs{\@coadvisorname}}}}\\%
        \fi
        \sanhao\durationpre\quad\underline{\makebox[20em][s]{
            \makebox[20em][c]{\fs{\@duration}}}}\\%
        \sanhao\addresspre\quad\underline{\makebox[20em][s]{
            \makebox[20em][c]{\fs{\@address}}}} \\
      \end{tabular}
      %\vfill
      %{\sanhao\today \par}
    \end{center}
  \end{titlepage}
}
\fi
\newcommand{\englishcover}{
  \thispagestyle{empty}%
  \begin{center}
    ~\\
      {\fontsize{24pt}{28pt}\selectfont
        \MakeUppercase{\@titleeng}\\
        \MakeUppercase{\@subtitleeng}
        \par
      }

      \vspace{1.0in}%
      {\fontsize{16pt}{24pt}\selectfont
        {A
          \ifmasterdegree
          Thesis
          \else
          Dissertation
          \fi
        submitted to}\\
        {\universitynameeng}\\
        {For the Academic Degree of \academicdegreeeng}\\
        {\vspace{0.5in}}%
        {BY}\\
        {\@authoreng}\\
        {\vspace{0.5in}}%
        {Supervised by:}\\
        {\@advisortitleeng\  \@advisornameeng}\\
        \ifbcoadvisor
        {and}\\
        {\@coadvisortitleeng\  \@coadvisornameeng}\\
        \else
        \fi
        {\vspace{0.5in}}%
        {\vfill}
        {\@departmenteng}\\
        {\universitynameeng}\\
        {\todayeng}
        \par
      }%
    \end{center}%
  \stcleardp%
}
\newenvironment{abstract}[1]
{
  \newcommand{\@keywords}{#1}
  \hypersetup{pdfkeywords={\@keywords}}
  \stcleardp
  \phantomsection
  \pagestyle{seunolabelstyle}
  \addcontentsline{toc}{chapter}{摘\quad \quad 要}
  \ifbachelordegree
  % \renewcommand{\newtitleline}{\\}
  %\chapter*{\@title}

  % \vspace*{10pt}
  % \begin{center}
  %   \xiaoerhao \hei\@title
  % \end{center}

  % \begin{flushright}
  %   \begin{tabular}[h]{ll}
  %     \@studentid & \kai\@author \\
  %     \advisorpre & \kai\@advisorname \\
  %   \end{tabular}
  % \end{flushright}
  %\vspace*{30pt}

  \begin{center}
    \sanhao \hei \textbf{摘}\quad \quad \textbf{要}
  \end{center}
  %\vspace*{30pt}

  \par
  % \else
  % \chapter*{摘\quad \quad 要}
  % \fi
  \song
  \ifbachelordegree
  \xiaosihao

  \else
  \wuhao
  \fi
}{
  \\[1\baselineskip]
  关键词： \@keywords
}
\newenvironment{englishabstract}[1]
{
  \newcommand{\@keywords}{#1}
  \stcleardp
  \phantomsection
  \setcounter{page}{2}
  % \thispagestyle{seunolabelstyle}
  \addcontentsline{toc}{chapter}{ABSTRACT}
  \ifbachelordegree

  % \vspace*{10pt}
  \begin{center}
    \xiaoerhao \textbf{\@titleeng}
  \end{center}

  % \begin{flushright}
  %   \begin{tabular}[h]{ll}
  %     \@studentid & \@authoreng \\
  %     Advisor & \@advisornameeng \\
  %   \end{tabular}
  % \end{flushright}

  \begin{center}
    {\sanhao \textbf{ABSTRACT}}
  \end{center}
  \par
  \else
  \chapter*{ABSTRACT}
  \fi
  \song
  \ifbachelordegree
  \xiaosihao
  \else
  \wuhao
  \fi
}{
  \\[1\baselineskip]
  KEY WORDS: \@keywords
}


\let\tableofcontents@orig=\tableofcontents

\renewcommand{\tableofcontents}
{
  \stcleardp
  {\linespacing{1.5}%
  \song\xiaosihao%
  \phantomsection
  \addcontentsline{toc}{chapter}{目\quad \quad 录}
  % \thispagestyle{seunolabelstyle}
  
  \renewcommand{\contentsname}{\textbf{目}\quad \quad \textbf{录}}
  % \titlecontents{chapter}{\itseries}
  \tableofcontents@orig \par
  \clearpage} %目录部分字体可采用小四号宋体。
  % \chapter*{目\ \ \ \ 录}

  \renewpagestyle{plain}{\pagestyle{seustyle}}
}
\newenvironment{terminology}[1][\@terminologytitle]
{
  \stcleardp
  \phantomsection
  \addcontentsline{toc}{chapter}{#1}
  \chapter*{#1}
  \song\xiaosihao
}{
  \par
}
\newenvironment{Main}
{
  \mainmatter % 开始正文部分
  \song\xiaosihao\linespacing{1.5} % 小四号宋体
}{
  \par
  \backmatter % 开始正文之后的部分
}
\renewcommand{\textfraction}{0.07}
\renewcommand{\topfraction}{0.9}
\renewcommand{\bottomfraction}{0.8}
\renewcommand{\floatpagefraction}{0.85}
\setcounter{totalnumber}{6}
\let\originalcite=\cite
\renewcommand{\cite}[1]{\textsuperscript{\originalcite{#1}}}
\renewcommand{\citet}[1]{\originalcite{#1}}

% my change
\newif\ifthu@bachelor
\def\thudot{\ifthu@bachelor\else\unskip.\fi}
\bibliographystyle{thuthesis}
% my change
%\renewcommand\@biblabel[1]{#1}  % Reference list option change from [1] to 1
\let\orig@bibliography=\bibliography
\renewcommand{\bibliography}[1]{
  \orig@bibliography{#1}
  \pagestyle{seustyle}\clearpage % 使参考文献使用 seunolabel 页面样式
  \pagestyle{seufrontstyle}} % 恢复原有样式给后续部分
\newenvironment{Acknowledgement}[1][\@acknowledgementtitle]
{%
  \stcleardp
  \phantomsection
  \addcontentsline{toc}{chapter}{\@acknowledgementtitletoc}
  \chapter*{#1}
  \song\xiaosihao %my change
}{\par}
\newenvironment{Appendix}
{%
  \pagestyle{seustyle}
  \@mainmattertrue
  \appendix
  \titlecontentsappendix
}{\@mainmatterfalse\par
  \thispagestyle{seustyle}\clearpage
}
\newenvironment{Resume}[1][\@resumetitle]
{%
  \stcleardp
  \phantomsection
  \addcontentsline{toc}{chapter}{\@resumetitletoc}
  \chapter*{#1}
  \song\wuhao
}{\par}
\newcommand{\backcover}{
  \pagestyle{empty}
  \clearpage
  \begin{picture}(0,0)(20,650)
    {\includegraphics[width=\textwidth,bb=0 0 2424 3508]{figures/back-cover.png}}
  \end{picture}
}
\ifbachelordegree
\renewcommand{\backcover}{}
\fi


\endinput
%%
%% End of file `seuthesis.cls'.
