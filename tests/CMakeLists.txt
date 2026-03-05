add_executable(EndlessSkyTests)
if(UNIX AND NOT APPLE)
	set_target_properties(EndlessSkyTests PROPERTIES OUTPUT_NAME "endless-sky-tests")
endif()

if(NOT MSVC)
	target_compile_options(EndlessSkyTests PUBLIC
		"-Wall" "-pedantic-errors" "-Wold-style-cast" "-fno-rtti")
endif()

find_package(Catch2 CONFIG REQUIRED)

# Every source file (and header file) should be listed here.
# If you add a new file, add it to this list.
target_sources(EndlessSkyTests PRIVATE
	unit/include/datanode-factory.h
	unit/include/es-test.hpp
	unit/include/logger-output.h
	unit/include/output-capture.hpp
	unit/src/comparators/test_byGivenOrder.cpp
	unit/src/comparators/test_byName.cpp
	unit/src/helpers/datanode-factory.cpp
	unit/src/helpers/logger-output.cpp
	unit/src/test_account.cpp
	unit/src/test_ai.cpp
	unit/src/test_alertLabel.cpp
	unit/src/test_ammoDisplay.cpp
	unit/src/test_angle.cpp
	unit/src/test_animate.cpp
	unit/src/test_armament.cpp
	unit/src/test_asteroidField.cpp
	unit/src/test_BankPanel.cpp
	unit/src/test_bitset.cpp
	unit/src/test_body.cpp
	unit/src/test_BoardingPanel.cpp
	unit/src/test_bookEntry.cpp
	unit/src/test_camera.cpp
	unit/src/test_captureOdds.cpp
	unit/src/test_cargoHold.cpp
	unit/src/test_categoryList.cpp
	unit/src/test_CategoryType.cpp
	unit/src/test_ClickZone.cpp
	unit/src/test_collision.cpp
	unit/src/test_collisionSet.cpp
	unit/src/test_CollisionType.cpp
	unit/src/test_color.cpp
	unit/src/test_command.cpp
	unit/src/test_conditionAssignments.cpp
	unit/src/test_conditionEntry.cpp
	unit/src/test_conditionSet.cpp
	unit/src/test_conditionsStore.cpp
	unit/src/test_conversation.cpp
	unit/src/test_ConversationPanel.cpp
	unit/src/test_coreStartData.cpp
	unit/src/test_customEvents.cpp
	unit/src/test_damageProfile.cpp
	unit/src/test_datafile.cpp
	unit/src/test_datanode.cpp
	unit/src/test_datawriter.cpp
	unit/src/test_date.cpp
	unit/src/test_depreciation.cpp
	unit/src/test_dialogSettings.cpp
	unit/src/test_DialogPanel.cpp
	unit/src/test_dictionary.cpp
	unit/src/test_distance_calculation_settings.cpp
	unit/src/test_distanceMap.cpp
	unit/src/test_distribution.cpp
	unit/src/test_drawable.cpp
	unit/src/test_effect.cpp
	unit/src/test_endpoint.cpp
	unit/src/test_engine.cpp
	unit/src/test_entity.cpp
	unit/src/test_escortDisplay.cpp
	unit/src/test_esuuid.cpp
	unit/src/test_exclusiveItem.cpp
	unit/src/test_files.cpp
	unit/src/test_firecommand.cpp
	unit/src/test_fighterHitHelper.cpp
	unit/src/test_fleet.cpp
	unit/src/test_fleetCargo.cpp
	unit/src/test_flotsam.cpp
	unit/src/test_formationPattern.cpp
	unit/src/test_formationPositioner.cpp
	unit/src/test_frameTimer.cpp
	unit/src/test_galaxy.cpp
	unit/src/test_gameAction.cpp
	unit/src/test_gameData.cpp
	unit/src/test_gameEvent.cpp
	unit/src/test_GameLoadingPanel.cpp
	unit/src/test_gamerules.cpp
	unit/src/test_GamerulesPanel.cpp
	unit/src/test_gameVersion.cpp
	unit/src/test_gameWindow.cpp
	unit/src/test_government.cpp
	unit/src/test_hailPanel.cpp
	unit/src/test_hardpoint.cpp
	unit/src/test_hasher.cpp
	unit/src/test_hazard.cpp
	unit/src/test_HiringPanel.cpp
	unit/src/test_infoPanelState.cpp
	unit/src/test_information.cpp
	unit/src/test_interface.cpp
	unit/src/test_itemInfoDisplay.cpp
	unit/src/test_JumpType.cpp
	unit/src/test_loadingCircle.cpp
	unit/src/test_LoadPanel.cpp
	unit/src/test_locationFilter.cpp
	unit/src/test_LogbookPanel.cpp
	unit/src/test_logger.cpp
	unit/src/test_main.cpp
	unit/src/test_MainPanel.cpp
	unit/src/test_MapDetailPanel.cpp
	unit/src/test_MapOutfitterPanel.cpp
	unit/src/test_mapPanel.cpp
	unit/src/test_mapPlanetCard.cpp
	unit/src/test_mapSalesPanel.cpp
	unit/src/test_mapShipyardPanel.cpp
	unit/src/test_menuAnimationPanel.cpp
	unit/src/test_menuPanel.cpp
	unit/src/test_message.cpp
	unit/src/test_messageLogPanel.cpp
	unit/src/test_messages.cpp
	unit/src/test_minable.cpp
	unit/src/test_minableDamageDealt.cpp
	unit/src/test_miniMap.cpp
	unit/src/test_mission.cpp
	unit/src/test_missionAction.cpp
	unit/src/test_missionPanel.cpp
	unit/src/test_missionTimer.cpp
	unit/src/test_mortgage.cpp
	unit/src/test_MouseButton.cpp
	unit/src/test_news.cpp
	unit/src/test_npc.cpp
	unit/src/test_npcAction.cpp
	unit/src/test_optionalInputDialogPanel.cpp
	unit/src/test_outfit.cpp
	unit/src/test_outfitInfoDisplay.cpp
	unit/src/test_outfitterPanel.cpp
	unit/src/test_panel.cpp
	unit/src/test_Paragraphs.cpp
	unit/src/test_person.cpp
	unit/src/test_personality.cpp
	unit/src/test_phrase.cpp
	unit/src/test_planet.cpp
	unit/src/test_planetLabel.cpp
	unit/src/test_planetPanel.cpp
	unit/src/test_playerInfo.cpp
	unit/src/test_playerInfoPanel.cpp
	unit/src/test_plugins.cpp
	unit/src/test_point.cpp
	unit/src/test_politics.cpp
	unit/src/test_port.cpp
	unit/src/test_preferences.cpp
	unit/src/test_preferencesPanel.cpp
	unit/src/test_printData.cpp
	unit/src/test_projectile.cpp
	unit/src/test_radar.cpp
	unit/src/test_raidFleet.cpp
	unit/src/test_random.cpp
	unit/src/test_randomEvent.cpp
	unit/src/test_rectangle.cpp
	unit/src/test_renderBuffer.cpp
	unit/src/test_routeEdge.cpp
	unit/src/test_routePlan.cpp
	unit/src/test_sale.cpp
	unit/src/test_savedGame.cpp
	unit/src/test_screen.cpp
	unit/src/test_scrollBar.cpp
	unit/src/test_scrollVar.cpp
	unit/src/test_set.cpp
	unit/src/test_ship.cpp
	unit/src/test_shipEvent.cpp
	unit/src/test_shipInfoDisplay.cpp
	unit/src/test_shipInfoPanel.cpp
	unit/src/test_shipJumpNavigation.cpp
	unit/src/test_shipManager.cpp
	unit/src/test_shipNameDialogPanel.cpp
	unit/src/test_shipyardPanel.cpp
	unit/src/test_shop.cpp
	unit/src/test_shopPanel.cpp
	unit/src/test_spaceportPanel.cpp
	unit/src/test_startConditions.cpp
	unit/src/test_startConditionsPanel.cpp
	unit/src/test_stellarObject.cpp
	unit/src/test_stringInterner.cpp
	unit/src/test_swizzle.cpp
	unit/src/test_system.cpp
	unit/src/test_SystemEntry.cpp
	unit/src/test_taskQueue.cpp
	unit/src/test_template.txt
	unit/src/test_textArea.cpp
	unit/src/test_textReplacements.cpp
	unit/src/test_tooltip.cpp
	unit/src/test_trade.cpp
	unit/src/test_tradingPanel.cpp
	unit/src/test_ui.cpp
	unit/src/test_universeObjects.cpp
	unit/src/test_variant.cpp
	unit/src/test_visual.cpp
	unit/src/test_weapon.cpp
	unit/src/test_weather.cpp
	unit/src/test_weightedList.cpp
	unit/src/test_wormhole.cpp
	unit/src/test_WormholeStrategy.cpp
	unit/src/test_zipFile.cpp
	unit/src/text/test_alignment.cpp
	unit/src/text/test_displaytext.cpp
	unit/src/text/test_format.cpp
	unit/src/text/test_layout.cpp
	unit/src/text/test_truncate.cpp
)

list(APPEND INTEGRATION_TESTS
	integration/config/plugins/integration-tests/data/tests/tests_afterburn_flight.txt
	integration/config/plugins/integration-tests/data/tests/tests_capture_override.txt
	integration/config/plugins/integration-tests/data/tests/tests_common.txt
	integration/config/plugins/integration-tests/data/tests/tests_conditional_choice.txt
	integration/config/plugins/integration-tests/data/tests/tests_disown.txt
	integration/config/plugins/integration-tests/data/tests/tests_flagship_attribute_autoconditions.txt
	integration/config/plugins/integration-tests/data/tests/tests_flagship_model_condition.txt
	integration/config/plugins/integration-tests/data/tests/tests_flagship_planet_attribute.txt
	integration/config/plugins/integration-tests/data/tests/tests_framework.txt
	integration/config/plugins/integration-tests/data/tests/tests_hyperjumps_to_autocondition.txt
	integration/config/plugins/integration-tests/data/tests/tests_illegal_atrocity.txt
	integration/config/plugins/integration-tests/data/tests/tests_job_board_missions.txt
	integration/config/plugins/integration-tests/data/tests/tests_landing.txt
	integration/config/plugins/integration-tests/data/tests/tests_take_ships.txt
	integration/config/plugins/integration-tests/data/tests/tests_timers.txt
	integration/config/plugins/integration-tests/data/tests/tests_save_load.txt
	integration/config/plugins/integration-tests/data/tests/tests_ship_sale.txt
	integration/config/plugins/integration-tests/data/tests/tests_shipyard_outfitter_missions.txt
	integration/config/plugins/integration-tests/data/tests/tests_store_outfits_on_take_off.txt
	integration/config/plugins/integration-tests/data/tests/tests_to_accept.txt
	integration/config/plugins/integration-tests/data/tests/tests_wormhole_navigation.txt
	integration/config/plugins/integration-tests/data/tests/tests_installed_plugin.txt
)

target_include_directories(EndlessSkyTests PRIVATE unit/include)
target_link_libraries(EndlessSkyTests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(EndlessSkyTests PRIVATE ExternalLibraries $<TARGET_OBJECTS:EndlessSkyLib>)

target_compile_options(EndlessSkyTests PUBLIC ${SANITIZER_OPTS})
target_link_options(EndlessSkyTests PUBLIC ${SANITIZER_OPTS})

# CTest support for our unit tests.
add_test(NAME unit COMMAND EndlessSkyTests)
set_tests_properties(unit PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" LABELS unit)
add_test(NAME benchmark COMMAND "$<TARGET_FILE:EndlessSkyTests>" [!benchmark])
set_tests_properties(benchmark PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" LABELS benchmark)

# Integration tests.
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/IntegrationTests_tests.cmake"
	COMMENT "Discover every integration test"
	COMMAND "${CMAKE_COMMAND}" -DES=$<TARGET_FILE:EndlessSky>
		-DBINARY_PATH=${CMAKE_CURRENT_BINARY_DIR}
		-DRESOURCE_PATH=${CMAKE_SOURCE_DIR}
		-DES_USE_OFFSCREEN=${ES_USE_OFFSCREEN}
		-P "${CMAKE_CURRENT_SOURCE_DIR}/integration/IntegrationTests.cmake"
	DEPENDS EndlessSky ${INTEGRATION_TESTS}
		"${CMAKE_CURRENT_SOURCE_DIR}/integration/IntegrationTests.cmake"
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" VERBATIM)
set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${CMAKE_CURRENT_BINARY_DIR}/IntegrationTests_tests.cmake")
add_custom_target(IntegrationTests ALL
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/IntegrationTests_tests.cmake"
	VERBATIM
)

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/IntegrationTests_tests.cmake")
	# If the target is not run, simply don't provide any integration tests instead of erroring out.
	file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/IntegrationTests_tests.cmake" "")
endif()

# Copy the MinGW runtime DLLs if necessary.
if(MINGW AND WIN32)
	foreach(FILE_PATH ${MINGW_RUNTIME})
		add_custom_command(TARGET EndlessSkyTests POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${FILE_PATH}" "$<TARGET_FILE_DIR:EndlessSkyTests>"
			COMMAND_EXPAND_LISTS VERBATIM)
	endforeach()
endif()
