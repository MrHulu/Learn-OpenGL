<div align="center">
  <h1>eCharts</h1>
  <p>通过Qt的QWebView使用eCharts提供数据可视化图表</p>
  <hr/>
</div>

<div id="dir"><h2>目录</h2>
<ul>
<li><a href="#introduce">eCharts介绍</a></li>
<li><a href="#introduction">如何引入到Qt项目中</a></li>
</ul>
</div>

<div id="introduction"><hr/>
  <h2>eCharts介绍</h2>
	<a href="echarts.apache.org">官网地址</a>
  	<p><img src="./Doc/Images/eChartsDownload.png" alt="在线定制"></p>
	<p>这里有下载好的： <a href="./Doc/eCharts图表">传送门</a></p>
</div>

<div id="introduce"><hr/>
  <h2>如何引入到Qt项目中</h2>
  <p><strong>引入webview模块</strong></p>
  <p>如果你使用的是qmake</p>
	  <pre><code>
    QT += webenginewidgets
    </code></pre>
  <p>如果你使用的是cmake</p>
	  <pre><code>
    find_package(Webenginewidgets)
    </code></pre>
</div>
