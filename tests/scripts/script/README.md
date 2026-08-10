# 用例设计原则
- 测试文件统一命名为 `test_*.py` 或 `*_test.py`
- 测试方法和函数统一命名为 `test_*()`
- 测试类命名应该以 `Test` 开头，并且不能带有 `__init__` 方法
- 所有的 pakege 必须要有 `__init__.py` 文件
- 每个 testsuite case 写在一个 `[testsuite]_test.py` 文件中，同一类型的测试用例放在 `[testsuite]_test` 文件夹
- 使用 `pytest.mark` 区分不同平台的 testcase
- 需要 skip 的 case，使用 `pytest.mark.skip` or `pytest.mark.skipif`
# 测试目录结构
- **script**：不同模块的测试脚本
- **utils**：通用方法函数
- **pytest.ini**：pytest的主配置文件，可以改变pytest的默认行为
- **conftest.py**：测试用例的一些 fixture 配置
- **requirements.txt**：测试使用的 python 三方库
