#include <gtest/gtest.h>

#include "levels/Level2.h"
#include <fstream>

using namespace godot;

WaveInfo createWaveInfo(std::vector<std::string> const &upgrades_p,
    std::vector<WaveUnitCount> const &mainUnits_p, int mainSteps_p,
    std::vector<WaveUnitCount> const &earlyUnits_p, int earlySteps_p,
    double dropCoef_p)
{
    WaveInfo info_l;

    info_l.upgrades = upgrades_p;

    info_l.mainWave.units = mainUnits_p;
    info_l.mainWave.steps = mainSteps_p;

    info_l.earlyWave.units = earlyUnits_p;
    info_l.earlyWave.steps = earlySteps_p;

    info_l.dropCoef = dropCoef_p;

    return info_l;
}

TEST(level2HeaderIOTest, simple)
{
    std::string const fileName_l = "tempfile.test";
    {
        level2::WaveLevelHeader header_l;

        // fill header
        {
            WavePoolInfo poolInfos_l;

            poolInfos_l.infos.push_back(createWaveInfo({"test1", "test2", "test3"},
                    {{"unit1", 12}, {"unit2", 5}}, 54,
                    {{"unit4", 3}, {"unit5", 52}}, 7,
                    1254));

            poolInfos_l.infos.push_back(createWaveInfo({"test4", "test5", "test6"},
                    {{"unit1", 2}, {"unit2", 7}}, 12,
                    {{"unit4", 12}, {"unit5", 5}}, 35,
                    0));

            header_l.tierWaveInfo.push_back(poolInfos_l);
        }
        {
            WavePoolInfo poolInfos_l;

            poolInfos_l.infos.push_back(createWaveInfo({"test7", "test8", "test9"},
                    {{"unitA", 7}}, 10,
                    {{"unitC", 1}}, 3,
                    25));

            poolInfos_l.infos.push_back(createWaveInfo({"test10", "test11", "test12"},
                    {{"unit1", 6}, {"unit2", 3}}, 1,
                    {{"unit4", 7}, {"unit5", 5}}, 2,
                    786));

            header_l.tierWaveInfo.push_back(poolInfos_l);
        }

        std::ofstream file_l(fileName_l, std::ios::out | std::ios::binary);

        level2::writeWaveLevelHeader(file_l, header_l);
    }

    {
        level2::WaveLevelHeader header_l;
        std::ifstream file_l(fileName_l, std::ios::in | std::ios::binary);
        level2::readWaveLevelHeader(file_l, header_l);

        ASSERT_EQ(2u, header_l.tierWaveInfo.size());

        ASSERT_EQ(2u, header_l.tierWaveInfo.at(0).infos.size());
            ASSERT_EQ(3u, header_l.tierWaveInfo.at(0).infos.at(0).upgrades.size());
                EXPECT_EQ("test1", header_l.tierWaveInfo.at(0).infos.at(0).upgrades.at(0));
                EXPECT_EQ("test2", header_l.tierWaveInfo.at(0).infos.at(0).upgrades.at(1));
                EXPECT_EQ("test3", header_l.tierWaveInfo.at(0).infos.at(0).upgrades.at(2));
            ASSERT_EQ(54, header_l.tierWaveInfo.at(0).infos.at(0).mainWave.steps);
            ASSERT_EQ(2u, header_l.tierWaveInfo.at(0).infos.at(0).mainWave.units.size());
                EXPECT_EQ("unit1", header_l.tierWaveInfo.at(0).infos.at(0).mainWave.units.at(0).model);
                EXPECT_EQ(12, header_l.tierWaveInfo.at(0).infos.at(0).mainWave.units.at(0).count);
                EXPECT_EQ("unit2", header_l.tierWaveInfo.at(0).infos.at(0).mainWave.units.at(1).model);
                EXPECT_EQ(5, header_l.tierWaveInfo.at(0).infos.at(0).mainWave.units.at(1).count);
            ASSERT_EQ(7, header_l.tierWaveInfo.at(0).infos.at(0).earlyWave.steps);
            ASSERT_EQ(2u, header_l.tierWaveInfo.at(0).infos.at(0).earlyWave.units.size());
                EXPECT_EQ("unit4", header_l.tierWaveInfo.at(0).infos.at(0).earlyWave.units.at(0).model);
                EXPECT_EQ(3, header_l.tierWaveInfo.at(0).infos.at(0).earlyWave.units.at(0).count);
                EXPECT_EQ("unit5", header_l.tierWaveInfo.at(0).infos.at(0).earlyWave.units.at(1).model);
                EXPECT_EQ(52, header_l.tierWaveInfo.at(0).infos.at(0).earlyWave.units.at(1).count);
            EXPECT_EQ(1254, header_l.tierWaveInfo.at(0).infos.at(0).dropCoef);

            ASSERT_EQ(3u, header_l.tierWaveInfo.at(0).infos.at(1).upgrades.size());
                EXPECT_EQ("test4", header_l.tierWaveInfo.at(0).infos.at(1).upgrades.at(0));
                EXPECT_EQ("test5", header_l.tierWaveInfo.at(0).infos.at(1).upgrades.at(1));
                EXPECT_EQ("test6", header_l.tierWaveInfo.at(0).infos.at(1).upgrades.at(2));
            ASSERT_EQ(12, header_l.tierWaveInfo.at(0).infos.at(1).mainWave.steps);
            ASSERT_EQ(2u, header_l.tierWaveInfo.at(0).infos.at(1).mainWave.units.size());
                EXPECT_EQ("unit1", header_l.tierWaveInfo.at(0).infos.at(1).mainWave.units.at(0).model);
                EXPECT_EQ(2, header_l.tierWaveInfo.at(0).infos.at(1).mainWave.units.at(0).count);
                EXPECT_EQ("unit2", header_l.tierWaveInfo.at(0).infos.at(1).mainWave.units.at(1).model);
                EXPECT_EQ(7, header_l.tierWaveInfo.at(0).infos.at(1).mainWave.units.at(1).count);
            ASSERT_EQ(35, header_l.tierWaveInfo.at(0).infos.at(1).earlyWave.steps);
            ASSERT_EQ(2u, header_l.tierWaveInfo.at(0).infos.at(1).earlyWave.units.size());
                EXPECT_EQ("unit4", header_l.tierWaveInfo.at(0).infos.at(1).earlyWave.units.at(0).model);
                EXPECT_EQ(12, header_l.tierWaveInfo.at(0).infos.at(1).earlyWave.units.at(0).count);
                EXPECT_EQ("unit5", header_l.tierWaveInfo.at(0).infos.at(1).earlyWave.units.at(1).model);
                EXPECT_EQ(5, header_l.tierWaveInfo.at(0).infos.at(1).earlyWave.units.at(1).count);
            EXPECT_EQ(0, header_l.tierWaveInfo.at(0).infos.at(1).dropCoef);

        ASSERT_EQ(2u, header_l.tierWaveInfo.at(1).infos.size());
            ASSERT_EQ(3u, header_l.tierWaveInfo.at(1).infos.at(0).upgrades.size());
                EXPECT_EQ("test7", header_l.tierWaveInfo.at(1).infos.at(0).upgrades.at(0));
                EXPECT_EQ("test8", header_l.tierWaveInfo.at(1).infos.at(0).upgrades.at(1));
                EXPECT_EQ("test9", header_l.tierWaveInfo.at(1).infos.at(0).upgrades.at(2));
            ASSERT_EQ(10, header_l.tierWaveInfo.at(1).infos.at(0).mainWave.steps);
            ASSERT_EQ(1u, header_l.tierWaveInfo.at(1).infos.at(0).mainWave.units.size());
                EXPECT_EQ("unitA", header_l.tierWaveInfo.at(1).infos.at(0).mainWave.units.at(0).model);
                EXPECT_EQ(7, header_l.tierWaveInfo.at(1).infos.at(0).mainWave.units.at(0).count);
            ASSERT_EQ(3, header_l.tierWaveInfo.at(1).infos.at(0).earlyWave.steps);
            ASSERT_EQ(1u, header_l.tierWaveInfo.at(1).infos.at(0).earlyWave.units.size());
                EXPECT_EQ("unitC", header_l.tierWaveInfo.at(1).infos.at(0).earlyWave.units.at(0).model);
                EXPECT_EQ(1, header_l.tierWaveInfo.at(1).infos.at(0).earlyWave.units.at(0).count);
            EXPECT_EQ(25, header_l.tierWaveInfo.at(1).infos.at(0).dropCoef);

            ASSERT_EQ(3u, header_l.tierWaveInfo.at(1).infos.at(1).upgrades.size());
                EXPECT_EQ("test10", header_l.tierWaveInfo.at(1).infos.at(1).upgrades.at(0));
                EXPECT_EQ("test11", header_l.tierWaveInfo.at(1).infos.at(1).upgrades.at(1));
                EXPECT_EQ("test12", header_l.tierWaveInfo.at(1).infos.at(1).upgrades.at(2));
            ASSERT_EQ(1, header_l.tierWaveInfo.at(1).infos.at(1).mainWave.steps);
            ASSERT_EQ(2u, header_l.tierWaveInfo.at(1).infos.at(1).mainWave.units.size());
                EXPECT_EQ("unit1", header_l.tierWaveInfo.at(1).infos.at(1).mainWave.units.at(0).model);
                EXPECT_EQ(6, header_l.tierWaveInfo.at(1).infos.at(1).mainWave.units.at(0).count);
                EXPECT_EQ("unit2", header_l.tierWaveInfo.at(1).infos.at(1).mainWave.units.at(1).model);
                EXPECT_EQ(3, header_l.tierWaveInfo.at(1).infos.at(1).mainWave.units.at(1).count);
            ASSERT_EQ(2, header_l.tierWaveInfo.at(1).infos.at(1).earlyWave.steps);
            ASSERT_EQ(2u, header_l.tierWaveInfo.at(1).infos.at(1).earlyWave.units.size());
                EXPECT_EQ("unit4", header_l.tierWaveInfo.at(1).infos.at(1).earlyWave.units.at(0).model);
                EXPECT_EQ(7, header_l.tierWaveInfo.at(1).infos.at(1).earlyWave.units.at(0).count);
                EXPECT_EQ("unit5", header_l.tierWaveInfo.at(1).infos.at(1).earlyWave.units.at(1).model);
                EXPECT_EQ(5, header_l.tierWaveInfo.at(1).infos.at(1).earlyWave.units.at(1).count);
            EXPECT_EQ(786, header_l.tierWaveInfo.at(1).infos.at(1).dropCoef);

    }

    std::remove(fileName_l.c_str());
}
