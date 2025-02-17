# Requirements

## Basic

- [ ] [10 分]基于 OpenGL，具有基本体素（立方体、球、圆柱、圆锥、多面棱柱、 多面棱台）的建模表达能力；

- [ ] [10 分]具有基本三维网格导入导出功能（建议 OBJ 格式）；

- [ ] [15 分]具有基本材质、纹理的显示和编辑能力；

- [ ] [10 分]具有基本几何变换功能（旋转、平移、缩放等）；

- [ ] [15 分]基本光照明模型要求，并实现基本的光源编辑（如调整光源的位置， 光强等参数）；

- [ ] [15 分]能对建模后场景进行漫游如 Zoom In/Out， Pan, Orbit, Zoom To Fit 等观察功能。

- [ ] [15 分]Awesomeness 指数：展示项目本身所独有的炫酷特点，包括但不限于 有感染力的视觉特效。

## Extra

- [ ] [4 分]漫游时可实时碰撞检测（**不包括 AABB**）
- [ ] [4 分]光照明模型细化，可任选实现实时阴影、Caustics、位移纹理、全局 光照明（光子跟踪）、辐射度、AO 叠加等
- [ ] [8 分（不依赖现有引擎）/ 4 分（依赖现有引擎）]采用 WebGL/iOS/Android 平台实现
- [ ] [7 分]与增强现实应用结合

# Features

1. 比较完整的天空部分的实现，主要包括：
   - 实时云层渲染
   - 天空盒以及一个写在 shader 里的太阳
   - lens flare 效果

![](https://github.com/Fairyland0902/FlightX/raw/master/screenshots/5.png)

2. 比较真实的海平面渲染，主要包括：
   - 基于 FFT 实现的海面 Simulation
   - 基于 GGX 实现的海面 Rendering

![](https://github.com/Fairyland0902/FlightX/raw/master/screenshots/6.png)