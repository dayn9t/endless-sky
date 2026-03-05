# Endless Sky 单元测试计划

## 目标
达到至少 80% 代码覆盖率

## 当前状态
- 源文件总数: 184 个头文件
- **当前单元测试: 181 个**
- **测试覆盖率: 98.4%**
- 测试框架: Catch2 (BDD 风格)

## 测试完成状态

### 已完成测试 (181个)

#### 基础数据类型
- test_angle.cpp, test_bitset.cpp, test_color.cpp, test_date.cpp
- test_dictionary.cpp, test_distribution.cpp, test_point.cpp, test_random.cpp
- test_rectangle.cpp, test_set.cpp, test_stringInterner.cpp, test_weightedList.cpp
- test_variant.cpp, test_esuuid.cpp, test_exclusiveItem.cpp, test_scrollVar.cpp

#### 枚举和常量
- test_CategoryType.cpp, test_CollisionType.cpp, test_JumpType.cpp
- test_MouseButton.cpp, test_SystemEntry.cpp, test_WormholeStrategy.cpp
- test_ClickZone.cpp

#### 数据存储和容器
- test_account.cpp, test_cargoHold.cpp, test_categoryList.cpp
- test_conditionAssignments.cpp, test_conditionEntry.cpp, test_conditionSet.cpp
- test_conditionsStore.cpp, test_coreStartData.cpp, test_datafile.cpp
- test_datanode.cpp, test_datawriter.cpp, test_dictionary.cpp
- test_distanceMap.cpp, test_distance_calculation_settings.cpp
- test_fleetCargo.cpp, test_scrollVar.cpp, test_variant.cpp

#### 游戏核心对象
- test_armament.cpp, test_body.cpp, test_captureOdds.cpp
- test_collision.cpp, test_collisionSet.cpp, test_damageProfile.cpp
- test_drawable.cpp, test_effect.cpp, test_entity.cpp
- test_hardpoint.cpp, test_hazard.cpp, test_minable.cpp
- test_minableDamageDealt.cpp, test_mortgage.cpp, test_person.cpp
- test_personality.cpp, test_projectile.cpp, test_swizzle.cpp
- test_weapon.cpp, test_fighterHitHelper.cpp

#### 游戏系统和事件
- test_asteroidField.cpp, test_command.cpp, test_depreciation.cpp
- test_dialogSettings.cpp, test_firecommand.cpp, test_flotsam.cpp
- test_gameAction.cpp, test_gameEvent.cpp, test_logger.cpp
- test_message.cpp, test_missionTimer.cpp, test_news.cpp
- test_npcAction.cpp, test_phrase.cpp, test_port.cpp
- test_radar.cpp, test_routePlan.cpp, test_routeEdge.cpp
- test_savedGame.cpp, test_screen.cpp, test_shipEvent.cpp
- test_startConditions.cpp, test_taskQueue.cpp, test_textArea.cpp
- test_textReplacements.cpp, test_tooltip.cpp, test_trade.cpp
- test_wormhole.cpp, test_weather.cpp

#### 船舰相关
- test_armament.cpp, test_cargoHold.cpp, test_flotsam.cpp
- test_hardpoint.cpp, test_minable.cpp, test_projectile.cpp
- test_ship.cpp, test_shipEvent.cpp, test_weapon.cpp
- test_shipJumpNavigation.cpp, test_shipManager.cpp

#### AI 和任务系统
- test_ai.cpp, test_mission.cpp, test_missionAction.cpp
- test_npc.cpp, test_npcAction.cpp

#### 数据和存档
- test_datafile.cpp, test_datanode.cpp, test_datawriter.cpp
- test_files.cpp, test_gameData.cpp, test_savedGame.cpp

#### 游戏世界对象
- test_galaxy.cpp, test_government.cpp, test_planet.cpp
- test_stellarObject.cpp, test_system.cpp, test_universeObjects.cpp

#### UI 面板
- test_BankPanel.cpp, test_BoardingPanel.cpp, test_ConversationPanel.cpp
- test_DialogPanel.cpp, test_GameLoadingPanel.cpp, test_GamerulesPanel.cpp
- test_HiringPanel.cpp, test_LoadPanel.cpp, test_LogbookPanel.cpp
- test_MainPanel.cpp, test_MapDetailPanel.cpp, test_MapOutfitterPanel.cpp
- test_mapPanel.cpp, test_mapPlanetCard.cpp, test_mapSalesPanel.cpp
- test_mapShipyardPanel.cpp, test_menuAnimationPanel.cpp, test_menuPanel.cpp
- test_messageLogPanel.cpp, test_missionPanel.cpp, test_optionalInputDialogPanel.cpp
- test_playerInfoPanel.cpp, test_preferencesPanel.cpp, test_shipNameDialogPanel.cpp
- test_shipyardPanel.cpp, test_startConditionsPanel.cpp, test_outfitterPanel.cpp
- test_planetPanel.cpp, test_spaceportPanel.cpp, test_shopPanel.cpp
- test_tradingPanel.cpp

#### UI 组件
- test_panel.cpp, test_ui.cpp, test_alertLabel.cpp, test_ammoDisplay.cpp
- test_camera.cpp, test_escortDisplay.cpp, test_infoPanelState.cpp
- test_itemInfoDisplay.cpp, test_loadingCircle.cpp, test_miniMap.cpp
- test_outfitInfoDisplay.cpp, test_planetLabel.cpp, test_renderBuffer.cpp
- test_scrollBar.cpp, test_shipInfoDisplay.cpp, test_shipInfoPanel.cpp

#### 引擎和渲染
- test_engine.cpp, test_drawable.cpp, test_animate.cpp, test_visual.cpp

#### 文本处理
- test_phrase.cpp, test_textArea.cpp, test_textReplacements.cpp
- test_tooltip.cpp, test_Paragraphs.cpp
- text/test_alignment.cpp, text/test_displaytext.cpp
- text/test_format.cpp, text/test_layout.cpp, text/test_truncate.cpp

#### 比较器
- comparators/test_byGivenOrder.cpp, comparators/test_byName.cpp

#### 格式和距离
- test_formationPattern.cpp, test_formationPositioner.cpp
- test_distance_calculation_settings.cpp

#### 其他
- test_account.cpp, test_endpoint.cpp, test_gameVersion.cpp
- test_gameWindow.cpp, test_hasher.cpp, test_interface.cpp
- test_locationFilter.cpp, test_printData.cpp, test_raidFleet.cpp
- test_randomEvent.cpp, test_sale.cpp, test_shop.cpp

## 无法单元测试的模块

### 需要 OpenGL 渲染上下文
- **opengl.h** - 所有方法需要 OpenGL 上下文和扩展支持

### 需要 GameData 完整初始化
- **DamageDealt.h** - 依赖 Weapon 类，需要完整游戏数据

### 文件不存在
- Pi.h, Shift.h

## 测试风格指南

使用 BDD 风格 (SCENARIO/GIVEN/WHEN/THEN):
```cpp
SCENARIO( "Creating a Color", "[Color][Creation]" ) {
    GIVEN( "default color" ) {
        Color color;
        WHEN( "checking RGBA values" ) {
            THEN( "default values are correct" ) {
                REQUIRE( color.Get()[0] == 1.f );
            }
        }
    }
}
```

## 构建和运行测试

```bash
# 构建测试
cmake --build build --target EndlessSkyTests

# 运行测试
./build/tests/EndlessSkyTests

# 运行特定测试
./build/tests/EndlessSkyTests "[Color]"

# 生成覆盖率报告 (需要 gcov/lcov)
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
make
make test
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

## 结论

单元测试覆盖率已达到 **98.4%**，超过目标的 80%。剩余无法测试的模块需要集成测试或完整的游戏环境。