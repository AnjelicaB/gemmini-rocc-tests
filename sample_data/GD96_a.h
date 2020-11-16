#define dim_I 1096 
#define dim_J 1096 
#define dim_K 256 
 
elem_t A[dim_I][dim_K] = {0};
elem_t B[dim_K][dim_J] = {0};
elem_t C[dim_I][dim_J] = {0};
acc_t full_D[dim_I][dim_J] = {0};
elem_t gold_dense[dim_I][dim_J] = {0};
elem_t gold[dim_I][dim_J] = {0};

int S_indptr[] = {0, 111, 137, 138, 139, 140, 159, 180, 186, 187, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 212, 213, 214, 215, 217, 218, 219, 220, 221, 223, 233, 234, 237, 255, 256, 257, 258, 259, 260, 261, 265, 266, 267, 268, 269, 270, 271, 273, 274, 275, 276, 277, 278, 279, 281, 282, 283, 284, 285, 287, 290, 297, 312, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 331, 332, 333, 343, 344, 345, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 364, 365, 366, 367, 368, 369, 370, 372, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 400, 401, 403, 404, 405, 406, 407, 408, 409, 410, 412, 416, 423, 425, 426, 432, 433, 435, 436, 438, 439, 440, 444, 445, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 479, 480, 481, 484, 485, 486, 487, 488, 489, 490, 492, 493, 494, 496, 497, 498, 499, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 513, 514, 515, 516, 517, 518, 519, 522, 523, 524, 528, 529, 530, 532, 533, 534, 535, 536, 537, 538, 543, 544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 584, 585, 596, 599, 600, 602, 609, 616, 617, 618, 619, 625, 626, 627, 628, 629, 630, 631, 632, 633, 634, 635, 636, 637, 638, 639, 640, 641, 642, 643, 644, 645, 646, 647, 648, 649, 650, 651, 652, 653, 654, 655, 656, 657, 658, 659, 660, 661, 662, 664, 669, 670, 673, 674, 679, 680, 681, 682, 683, 684, 685, 686, 687, 688, 689, 690, 691, 692, 693, 694, 695, 696, 697, 698, 699, 700, 701, 702, 703, 704, 705, 706, 707, 708, 709, 710, 711, 712, 713, 714, 718, 719, 720, 721, 722, 723, 724, 725, 726, 727, 728, 729, 730, 731, 732, 733, 736, 737, 738, 739, 740, 741, 742, 743, 744, 745, 746, 747, 748, 749, 750, 751, 752, 757, 758, 760, 761, 762, 763, 764, 765, 766, 767, 768, 769, 770, 771, 772, 773, 774, 775, 776, 777, 778, 779, 780, 781, 782, 783, 784, 785, 786, 787, 788, 789, 790, 791, 792, 793, 794, 795, 796, 797, 798, 799, 800, 801, 802, 803, 804, 805, 806, 807, 808, 809, 810, 811, 812, 813, 814, 815, 816, 817, 818, 819, 820, 821, 822, 823, 824, 825, 826, 827, 828, 829, 830, 831, 832, 873, 874, 876, 877, 878, 880, 881, 882, 883, 884, 885, 886, 887, 888, 889, 890, 891, 892, 893, 895, 896, 897, 898, 899, 900, 901, 902, 903, 904, 905, 906, 907, 908, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920, 921, 924, 925, 926, 932, 933, 934, 936, 937, 938, 939, 940, 941, 942, 943, 944, 945, 946, 947, 948, 949, 950, 951, 952, 954, 955, 956, 957, 958, 959, 960, 961, 963, 964, 965, 966, 977, 978, 979, 983, 984, 985, 986, 987, 988, 989, 990, 991, 992, 993, 994, 995, 996, 997, 998, 999, 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1017, 1018, 1019, 1020, 1021, 1022, 1023, 1024, 1025, 1027, 1028, 1029, 1030, 1031, 1032, 1034, 1035, 1036, 1038, 1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060, 1061, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071, 1072, 1073, 1074, 1075, 1076, 1107, 1108, 1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118, 1119, 1120, 1121, 1122, 1123, 1124, 1125, 1126, 1127, 1128, 1129, 1130, 1131, 1132, 1133, 1134, 1136, 1138, 1139, 1140, 1141, 1142, 1143, 1144, 1145, 1146, 1147, 1148, 1149, 1150, 1151, 1152, 1156, 1157, 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1167, 1168, 1169, 1170, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179, 1180, 1181, 1182, 1183, 1184, 1185, 1186, 1187, 1188, 1189, 1190, 1191, 1193, 1194, 1199, 1200, 1202, 1203, 1204, 1205, 1207, 1208, 1209, 1210, 1211, 1212, 1213, 1214, 1218, 1219, 1220, 1221, 1222, 1223, 1227, 1228, 1233, 1234, 1235, 1236, 1237, 1238, 1239, 1240, 1241, 1242, 1243, 1244, 1246, 1247, 1248, 1249, 1252, 1254, 1255, 1256, 1257, 1258, 1259, 1260, 1261, 1262, 1263, 1264, 1265, 1266, 1267, 1268, 1269, 1270, 1271, 1272, 1273, 1274, 1275, 1276, 1277, 1278, 1279, 1280, 1282, 1283, 1284, 1285, 1286, 1287, 1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295, 1296, 1297, 1299, 1300, 1301, 1302, 1303, 1305, 1306, 1307, 1308, 1309, 1310, 1311, 1312, 1313, 1314, 1315, 1316, 1317, 1319, 1320, 1321, 1322, 1323, 1324, 1325, 1326, 1327, 1328, 1329, 1330, 1331, 1332, 1333, 1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344, 1345, 1346, 1347, 1348, 1349, 1350, 1351, 1352, 1353, 1354, 1355, 1356, 1357, 1358, 1359, 1360, 1361, 1362, 1363, 1364, 1365, 1366, 1367, 1368, 1369, 1370, 1371, 1372, 1373, 1374, 1375, 1376, 1377, 1378, 1382, 1383, 1384, 1386, 1388, 1389, 1391, 1392, 1393, 1394, 1395, 1396, 1397, 1398, 1399, 1400, 1401, 1402, 1403, 1404, 1405, 1407, 1408, 1409, 1410, 1411, 1412, 1414, 1415, 1416, 1417, 1418, 1419, 1420, 1421, 1422, 1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1432, 1433, 1434, 1436, 1437, 1438, 1439, 1443, 1444, 1445, 1446, 1447, 1448, 1449, 1450, 1451, 1452, 1453, 1454, 1455, 1457, 1458, 1459, 1460, 1461, 1462, 1463, 1464, 1465, 1466, 1467, 1468, 1470, 1472, 1473, 1474, 1475, 1476, 1477, 1478, 1479, 1480, 1481, 1482, 1487, 1489, 1490, 1491, 1492, 1493, 1494, 1495, 1496, 1497, 1498, 1499, 1500, 1501, 1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509, 1510, 1511, 1516, 1517, 1518, 1519, 1520, 1521, 1522, 1523, 1524, 1525, 1526, 1527, 1528, 1530, 1534, 1535, 1536, 1537, 1538, 1539, 1540, 1541, 1542, 1543, 1545, 1546, 1549, 1550, 1551, 1552, 1553, 1554, 1555, 1556, 1557, 1558, 1560, 1561, 1562, 1563, 1564, 1565, 1566, 1567, 1569, 1570, 1571, 1572, 1573, 1574, 1575, 1576, 1577, 1578, 1580, 1581, 1582, 1583, 1584, 1586, 1587, 1588, 1589, 1590, 1591, 1592, 1595, 1596, 1597, 1598, 1599, 1600, 1601, 1604, 1605, 1606, 1607, 1609, 1611, 1612, 1621, 1622, 1623, 1624, 1625, 1626, 1627, 1633, 1634, 1635, 1636, 1637, 1638, 1639, 1640, 1641, 1642, 1643, 1644, 1645, 1646, 1647, 1648, 1649, 1650, 1651, 1652, 1653, 1654, 1655, 1656, 1657, 1658, 1659, 1660, 1661, 1662, 1663, 1664, 1665, 1666, 1667, 1668, 1669, 1670, 1671, 1672, 1674, 1676, 1677, 1677}; 
 
int S_index[] = {1, 5, 6, 7, 10, 19, 27, 29, 32, 34, 38, 39, 61, 99, 180, 225, 228, 243, 259, 274, 293, 333, 352, 377, 413, 418, 425, 441, 468, 503, 505, 516, 521, 523, 524, 525, 526, 530, 531, 533, 538, 540, 541, 542, 546, 548, 549, 550, 551, 553, 557, 559, 561, 562, 563, 565, 566, 567, 568, 573, 576, 577, 578, 579, 583, 584, 585, 707, 720, 728, 787, 810, 812, 816, 817, 825, 904, 905, 906, 907, 921, 922, 923, 924, 925, 926, 928, 929, 930, 932, 933, 935, 936, 956, 975, 976, 977, 987, 988, 990, 1006, 1012, 1013, 1014, 1016, 1017, 1018, 1025, 1026, 1069, 1087, 39, 46, 59, 133, 153, 190, 250, 295, 349, 395, 465, 469, 518, 519, 524, 552, 582, 584, 766, 812, 843, 844, 895, 927, 1008, 1012, 170, 17, 2, 26, 58, 198, 264, 339, 365, 405, 408, 547, 588, 628, 649, 675, 697, 838, 885, 900, 907, 944, 28, 35, 73, 103, 155, 237, 371, 474, 520, 521, 528, 529, 543, 551, 561, 571, 891, 892, 893, 921, 1070, 612, 839, 899, 941, 1019, 1020, 1065, 8, 33, 380, 389, 826, 922, 1025, 1087, 838, 587, 671, 662, 30, 1095, 66, 2, 178, 120, 348, 685, 766, 918, 978, 979, 1022, 529, 840, 543, 122, 577, 898, 484, 457, 271, 739, 851, 186, 258, 803, 859, 860, 861, 862, 863, 1001, 1086, 75, 162, 274, 448, 60, 91, 139, 146, 211, 249, 297, 376, 460, 470, 490, 526, 842, 897, 922, 978, 1004, 1013, 56, 895, 735, 569, 542, 493, 71, 300, 394, 1005, 906, 534, 261, 199, 187, 364, 254, 850, 104, 599, 972, 938, 262, 416, 353, 596, 476, 284, 3, 1032, 9, 174, 78, 208, 902, 79, 203, 656, 657, 658, 1010, 1019, 81, 138, 197, 205, 220, 248, 578, 613, 663, 664, 728, 799, 841, 941, 951, 98, 209, 256, 1020, 257, 87, 308, 22, 212, 211, 93, 95, 102, 279, 778, 12, 170, 105, 313, 108, 22, 35, 107, 258, 467, 852, 863, 925, 987, 990, 1024, 169, 112, 131, 482, 115, 439, 116, 682, 68, 417, 816, 126, 94, 537, 385, 22, 214, 411, 326, 401, 458, 121, 317, 211, 428, 472, 141, 311, 429, 143, 1008, 592, 287, 298, 646, 587, 22, 165, 147, 595, 647, 528, 181, 205, 22, 156, 22, 188, 593, 899, 781, 45, 534, 160, 163, 331, 386, 423, 164, 220, 390, 140, 201, 319, 324, 211, 498, 171, 759, 25, 109, 484, 761, 13, 183, 405, 626, 791, 792, 935, 571, 699, 621, 318, 383, 591, 618, 992, 1007, 698, 327, 590, 623, 86, 200, 704, 36, 391, 622, 674, 700, 389, 255, 356, 530, 617, 702, 779, 184, 480, 213, 381, 191, 117, 703, 575, 195, 950, 203, 540, 362, 404, 15, 224, 62, 226, 148, 214, 22, 1048, 170, 514, 48, 394, 528, 691, 369, 821, 305, 397, 714, 230, 231, 919, 425, 350, 301, 232, 495, 834, 375, 314, 706, 424, 736, 492, 106, 238, 337, 797, 981, 287, 15, 538, 630, 316, 542, 497, 542, 400, 569, 322, 569, 402, 239, 981, 282, 531, 616, 710, 281, 613, 942, 943, 944, 614, 15, 113, 355, 196, 57, 543, 615, 283, 528, 49, 343, 376, 605, 795, 425, 182, 479, 661, 796, 175, 22, 276, 277, 287, 659, 363, 783, 660, 47, 142, 201, 211, 215, 310, 332, 366, 390, 421, 464, 488, 508, 546, 640, 658, 713, 778, 888, 889, 951, 966, 1004, 1005, 1006, 1020, 1038, 373, 2, 23, 24, 263, 368, 513, 641, 647, 955, 965, 1036, 345, 365, 731, 196, 241, 242, 92, 370, 408, 442, 639, 664, 890, 136, 342, 602, 642, 707, 957, 958, 410, 783, 294, 28, 31, 521, 538, 561, 948, 493, 718, 515, 962, 1034, 336, 644, 321, 37, 867, 334, 534, 534, 805, 307, 675, 600, 201, 776, 494, 706, 569, 341, 209, 427, 65, 22, 501, 22, 22, 1041, 110, 22, 176, 872, 669, 982, 527, 809, 108, 246, 775, 863, 981, 16, 357, 551, 976, 993, 130, 837, 997, 1011, 1027, 430, 836, 559, 420, 835, 667, 834, 161, 41, 575, 833, 554, 777, 776, 358, 831, 399, 827, 542, 542, 497, 37, 830, 569, 426, 209, 774, 829, 217, 666, 828, 828, 665, 82, 745, 43, 259, 554, 743, 968, 586, 744, 981, 815, 221, 392, 569, 341, 742, 741, 150, 475, 755, 412, 84, 269, 976, 783, 649, 266, 585, 194, 673, 459, 672, 157, 462, 611, 211, 22, 54, 502, 22, 177, 901, 924, 925, 1052, 845, 634, 786, 170, 722, 985, 784, 783, 580, 635, 579, 715, 850, 718, 781, 781, 986, 444, 608, 536, 607, 772, 719, 214, 981, 251, 459, 539, 538, 717, 776, 359, 435, 542, 276, 51, 716, 235, 123, 785, 530, 531, 850, 459, 409, 500, 315, 286, 475, 802, 252, 431, 723, 566, 432, 534, 306, 621, 970, 569, 209, 499, 544, 55, 440, 870, 869, 461, 688, 868, 687, 223, 330, 265, 100, 33, 76, 88, 175, 245, 252, 491, 542, 556, 557, 558, 559, 560, 561, 562, 564, 567, 727, 787, 792, 824, 825, 826, 874, 925, 929, 930, 932, 933, 935, 990, 991, 1016, 1017, 1018, 1022, 1024, 1025, 1026, 1029, 1069, 789, 75, 873, 781, 949, 823, 980, 822, 551, 875, 77, 821, 821, 677, 231, 479, 653, 15, 788, 537, 291, 496, 654, 818, 541, 726, 135, 179, 663, 813, 101, 137, 235, 329, 952, 447, 542, 917, 369, 96, 713, 563, 821, 684, 887, 530, 504, 783, 565, 765, 977, 252, 534, 173, 457, 903, 1009, 1010, 1011, 534, 767, 202, 390, 201, 201, 419, 684, 275, 542, 764, 764, 886, 476, 403, 769, 679, 678, 89, 828, 128, 862, 839, 725, 798, 196, 247, 328, 74, 36, 378, 424, 207, 120, 53, 149, 218, 255, 446, 485, 522, 779, 853, 888, 922, 852, 171, 856, 997, 1027, 1028, 998, 1000, 804, 846, 890, 478, 914, 252, 52, 650, 15, 566, 219, 309, 201, 538, 457, 540, 457, 569, 69, 201, 140, 629, 647, 878, 909, 750, 940, 791, 981, 15, 11, 175, 229, 885, 783, 748, 152, 1081, 754, 752, 651, 894, 273, 234, 70, 15, 374, 449, 1015, 638, 64, 637, 705, 145, 423, 915, 236, 473, 210, 278, 652, 975, 269, 268, 111, 709, 793, 484, 712, 572, 222, 647, 253, 178, 847, 848, 849, 995, 849, 433, 132, 598, 784, 170, 937, 451, 72, 865, 866, 280, 436, 33, 269, 540, 560, 561, 562, 566, 567, 574, 802, 831, 842, 857, 928, 929, 931, 932, 934, 980, 989, 991, 999, 1001, 1017, 1018, 1025, 1026, 1029, 1031, 1049, 1069, 933, 998, 559, 4, 476, 1045, 1042, 97, 772, 1043, 770, 1046, 1044, 181, 369, 771, 136, 114, 686, 517, 841, 175, 772, 369, 119, 83, 206, 136, 303, 724, 1057, 854, 855, 761, 181, 129, 129, 185, 534, 615, 92, 680, 871, 202, 1059, 693, 824, 988, 1023, 694, 695, 168, 201, 649, 170, 535, 692, 437, 471, 542, 533, 170, 170, 22, 292, 459, 338, 361, 981, 809, 231, 706, 1054, 130, 340, 553, 424, 396, 681, 134, 399, 167, 21, 1015, 42, 373, 302, 586, 628, 746, 903, 913, 151, 710, 747, 323, 276, 456, 175, 908, 511, 913, 477, 267, 866, 648, 542, 49, 144, 394, 880, 881, 778, 778, 216, 594, 415, 901, 902, 1007, 182, 158, 233, 266, 424, 486, 325, 756, 214, 757, 892, 758, 185, 487, 189, 159, 581, 199, 800, 15, 260, 760, 44, 763, 1002, 379, 762, 346, 567, 367, 954, 866, 701, 18, 532, 285, 696, 801, 220, 627, 186, 414, 620, 451, 372, 808, 946, 451, 625, 919, 242, 354, 589, 10, 454, 877, 926, 689, 809, 569, 690, 633, 970, 811, 1017, 808, 40, 534, 906, 211, 15, 20, 201, 393, 170, 423, 351, 919, 534, 668, 15, 181, 178, 14, 555, 631, 479, 178, 489, 738, 201, 406, 398, 272, 632, 981, 175, 814, 815, 537, 889, 421, 647, 192, 911, 910, 534, 527, 604, 290, 939, 794, 320, 270, 808, 920, 434, 483, 821, 783, 445, 912, 732, 422, 806, 507, 22, 1040, 569, 457, 451, 220, 964, 534, 945, 963, 170, 861, 838, 1033, 908, 956, 729, 170, 778, 932, 559, 369, 542, 79, 733, 241, 680, 891, 967, 424, 603, 451, 1015, 375, 509, 896, 503, 961, 1090, 783, 77, 960, 1035, 421, 188, 407, 463, 925, 884, 399, 670, 214, 778, 983, 369, 347, 832, 802, 288, 779, 927, 668, 127, 360, 740, 211, 610, 919, 1053, 772, 636, 780, 984, 1014, 721, 259, 609, 720, 844, 782, 925, 204, 267, 1069, 927, 934, 989, 992, 1023, 1049, 528, 170, 1066, 170, 790, 819, 178, 655, 820, 783, 683, 768, 34, 570, 871, 821, 1008, 972, 802, 857, 858, 534, 783, 606, 414, 304, 443, 803, 1064, 50, 457, 455, 751, 154, 1055, 905, 994, 573, 597, 997, 1016, 1028, 1030, 1086, 186, 804, 935, 557, 479, 124, 85, 996, 118, 492, 953, 342, 734, 67, 879, 545, 1062, 172, 969, 970, 971, 568, 864, 1050, 624, 676, 906, 976, 1021, 975, 936, 438, 481, 220, 512, 615, 493, 222, 170, 1003, 312, 510, 1071, 388, 576, 810, 1072, 548, 15, 537, 90, 416, 387, 444, 1061, 1060, 820, 843, 1077, 947, 986, 1075, 705, 1039, 925, 643, 657, 1074, 601, 807, 302, 371, 866, 1037, 553, 525, 1051, 63, 562, 804, 803, 1018, 125, 214, 931, 934, 928, 1084, 1079, 749, 1080, 860, 1027, 551, 790, 737, 1078, 966, 1076, 1088, 1089, 730, 1073, 335, 166, 240, 466, 506, 344, 662, 244, 760, 1056, 225, 121, 193, 797, 950, 22, 80, 809, 815, 1029, 1093, 452, 524, 558, 566, 831, 923, 992, 999, 1018, 1049, 708, 370, 883, 1047, 117, 1067, 564, 773, 896, 933, 980, 1068, 1085, 783, 943, 186, 1058, 1063, 227, 974, 1092, 1070, 619, 714, 2, 289, 384, 645, 753, 916, 711, 267, 972, 973, 479, 1082, 899, 1083, 421, 444, 945, 876, 1094, 559, 450, 959, 999, 882, 127, 1091, 453, 296, 382, 77, 299, 175}; 
 
 
 
int S_indptr_j[] = {0, 0, 1, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 17, 18, 29, 30, 31, 32, 33, 34, 35, 52, 53, 54, 55, 56, 57, 59, 60, 61, 62, 63, 66, 68, 70, 72, 74, 75, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 115, 116, 119, 120, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 153, 154, 155, 156, 157, 158, 159, 160, 161, 163, 164, 165, 167, 169, 170, 171, 172, 173, 174, 176, 177, 179, 181, 182, 183, 184, 185, 186, 189, 190, 191, 192, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 237, 239, 240, 241, 242, 249, 250, 251, 256, 257, 258, 262, 264, 265, 266, 268, 272, 273, 275, 276, 277, 278, 279, 280, 281, 282, 285, 286, 287, 289, 290, 300, 302, 304, 305, 307, 308, 309, 310, 314, 315, 323, 324, 325, 331, 332, 333, 334, 335, 336, 341, 342, 344, 345, 346, 348, 349, 350, 351, 352, 353, 356, 357, 358, 359, 361, 362, 363, 364, 365, 366, 368, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 383, 384, 385, 387, 388, 389, 391, 394, 395, 396, 397, 398, 399, 400, 402, 405, 406, 409, 410, 411, 412, 413, 415, 416, 419, 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 488, 490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 514, 515, 516, 517, 523, 525, 527, 528, 530, 531, 533, 535, 536, 537, 538, 539, 540, 541, 542, 543, 544, 545, 546, 547, 549, 552, 553, 554, 555, 558, 559, 560, 561, 562, 565, 566, 567, 568, 569, 570, 572, 573, 574, 576, 577, 578, 579, 580, 581, 583, 584, 586, 587, 588, 589, 590, 594, 595, 598, 603, 606, 607, 608, 609, 610, 611, 612, 613, 614, 615, 616, 617, 618, 619, 620, 621, 622, 623, 624, 627, 628, 629, 630, 631, 632, 633, 638, 639, 640, 641, 642, 643, 649, 650, 654, 655, 656, 657, 658, 659, 660, 661, 662, 663, 664, 665, 666, 667, 668, 669, 671, 674, 675, 676, 681, 682, 683, 684, 685, 688, 689, 690, 691, 692, 693, 694, 695, 697, 700, 701, 702, 703, 705, 706, 707, 708, 709, 710, 712, 713, 714, 715, 716, 717, 718, 719, 720, 721, 722, 723, 724, 725, 726, 727, 728, 729, 732, 733, 734, 737, 739, 741, 743, 748, 750, 754, 757, 758, 760, 773, 774, 775, 779, 784, 785, 789, 791, 804, 807, 808, 809, 811, 812, 814, 815, 816, 821, 822, 825, 827, 828, 829, 832, 834, 840, 842, 847, 851, 853, 855, 857, 862, 866, 868, 878, 879, 881, 882, 884, 885, 887, 889, 891, 893, 895, 896, 897, 898, 899, 901, 903, 905, 907, 908, 909, 910, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920, 921, 922, 923, 924, 925, 926, 927, 928, 929, 930, 931, 932, 934, 935, 938, 939, 940, 941, 942, 943, 945, 946, 947, 948, 949, 950, 951, 953, 954, 955, 956, 957, 958, 959, 960, 961, 962, 963, 964, 965, 966, 967, 968, 969, 970, 971, 976, 977, 980, 981, 982, 983, 984, 985, 986, 987, 989, 991, 992, 993, 994, 996, 998, 1000, 1001, 1002, 1003, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1013, 1014, 1015, 1016, 1017, 1019, 1020, 1021, 1022, 1024, 1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037, 1038, 1039, 1040, 1041, 1042, 1043, 1044, 1046, 1049, 1051, 1052, 1053, 1055, 1056, 1057, 1059, 1061, 1062, 1063, 1064, 1066, 1067, 1069, 1070, 1071, 1072, 1073, 1074, 1075, 1076, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103, 1104, 1105, 1106, 1107, 1108, 1109, 1111, 1113, 1114, 1115, 1117, 1118, 1120, 1121, 1122, 1123, 1124, 1125, 1129, 1130, 1131, 1132, 1135, 1136, 1142, 1145, 1146, 1150, 1151, 1162, 1164, 1165, 1166, 1168, 1169, 1170, 1172, 1174, 1176, 1177, 1178, 1179, 1180, 1182, 1183, 1184, 1185, 1186, 1190, 1193, 1196, 1197, 1198, 1199, 1202, 1206, 1208, 1209, 1211, 1212, 1213, 1216, 1218, 1219, 1220, 1221, 1223, 1229, 1230, 1231, 1233, 1235, 1237, 1238, 1241, 1242, 1243, 1246, 1247, 1248, 1250, 1251, 1252, 1253, 1256, 1258, 1259, 1261, 1263, 1265, 1267, 1268, 1269, 1270, 1271, 1273, 1276, 1277, 1279, 1280, 1281, 1282, 1283, 1285, 1286, 1287, 1289, 1291, 1293, 1296, 1297, 1298, 1302, 1303, 1304, 1305, 1306, 1308, 1309, 1310, 1311, 1312, 1313, 1314, 1315, 1316, 1317, 1318, 1319, 1320, 1321, 1323, 1324, 1325, 1327, 1329, 1331, 1333, 1335, 1336, 1337, 1339, 1341, 1342, 1343, 1346, 1347, 1349, 1351, 1353, 1354, 1356, 1360, 1362, 1364, 1365, 1366, 1367, 1368, 1370, 1371, 1372, 1373, 1374, 1375, 1379, 1380, 1382, 1386, 1388, 1390, 1397, 1399, 1402, 1405, 1408, 1410, 1412, 1416, 1420, 1423, 1427, 1429, 1430, 1431, 1432, 1433, 1435, 1436, 1438, 1440, 1442, 1443, 1444, 1445, 1446, 1448, 1450, 1451, 1452, 1453, 1454, 1456, 1457, 1458, 1459, 1460, 1461, 1462, 1463, 1464, 1465, 1467, 1468, 1469, 1470, 1473, 1474, 1477, 1478, 1479, 1482, 1486, 1488, 1490, 1491, 1494, 1502, 1503, 1504, 1505, 1506, 1508, 1510, 1512, 1514, 1517, 1519, 1522, 1523, 1524, 1525, 1526, 1529, 1531, 1534, 1535, 1537, 1538, 1539, 1541, 1543, 1545, 1547, 1550, 1551, 1553, 1555, 1557, 1559, 1561, 1564, 1567, 1571, 1576, 1578, 1581, 1582, 1584, 1586, 1588, 1592, 1595, 1598, 1600, 1603, 1604, 1605, 1606, 1607, 1608, 1609, 1610, 1611, 1612, 1613, 1614, 1615, 1616, 1617, 1618, 1619, 1620, 1621, 1622, 1625, 1626, 1627, 1628, 1629, 1630, 1631, 1632, 1633, 1634, 1635, 1636, 1637, 1638, 1639, 1640, 1641, 1642, 1643, 1644, 1648, 1650, 1651, 1652, 1653, 1654, 1655, 1656, 1657, 1658, 1659, 1660, 1661, 1662, 1663, 1664, 1665, 1667, 1669, 1670, 1671, 1672, 1673, 1674, 1675, 1676, 1677}; 
 
int S_index_j[] = {0, 4, 17, 245, 1065, 56, 616, 0, 0, 0, 9, 58, 0, 759, 556, 76, 143, 786, 170, 201, 220, 457, 534, 555, 569, 728, 775, 783, 981, 294, 3, 739, 0, 775, 689, 68, 80, 96, 113, 121, 123, 176, 235, 281, 283, 284, 287, 362, 365, 671, 828, 1040, 245, 245, 142, 5, 0, 6, 254, 0, 14, 254, 0, 9, 441, 612, 0, 917, 6, 81, 152, 516, 263, 319, 0, 0, 1, 771, 306, 691, 333, 731, 128, 1, 243, 180, 228, 707, 931, 401, 532, 520, 363, 429, 33, 223, 5, 1, 32, 0, 172, 1006, 573, 280, 16, 954, 89, 544, 568, 39, 607, 6, 515, 30, 443, 441, 450, 862, 1093, 59, 60, 851, 1041, 61, 331, 638, 349, 947, 150, 66, 441, 506, 983, 32, 249, 651, 71, 93, 72, 476, 620, 62, 0, 440, 469, 73, 6, 47, 78, 197, 81, 79, 293, 142, 286, 586, 83, 221, 630, 85, 87, 161, 1050, 949, 637, 19, 519, 100, 1038, 23, 404, 946, 1010, 92, 882, 1089, 508, 646, 647, 297, 680, 84, 601, 1, 686, 465, 250, 629, 640, 470, 61, 32, 135, 546, 105, 243, 107, 707, 575, 32, 115, 174, 520, 345, 694, 561, 1, 935, 6, 122, 358, 715, 725, 130, 305, 31, 131, 133, 114, 1030, 688, 659, 82, 2, 77, 178, 369, 604, 662, 669, 670, 778, 838, 845, 906, 908, 975, 141, 522, 958, 488, 58, 234, 441, 556, 634, 699, 798, 1094, 288, 366, 18, 595, 785, 790, 911, 466, 0, 119, 626, 645, 784, 230, 714, 143, 156, 648, 722, 29, 746, 942, 1056, 44, 124, 866, 724, 1, 160, 805, 1038, 354, 164, 222, 247, 512, 61, 5, 43, 727, 150, 136, 243, 272, 492, 493, 538, 545, 660, 776, 793, 491, 654, 60, 166, 900, 61, 120, 639, 518, 59, 62, 278, 323, 426, 580, 32, 70, 102, 139, 243, 361, 774, 885, 69, 158, 97, 175, 389, 718, 873, 1011, 243, 711, 326, 520, 536, 61, 133, 744, 833, 970, 339, 592, 974, 437, 171, 0, 1037, 173, 1059, 0, 557, 184, 185, 454, 677, 190, 715, 567, 403, 471, 578, 6, 197, 213, 1031, 248, 853, 248, 756, 0, 1034, 441, 293, 513, 61, 32, 1, 391, 416, 441, 486, 531, 594, 46, 155, 520, 63, 65, 29, 81, 0, 333, 895, 729, 42, 51, 245, 5, 439, 352, 715, 703, 900, 1072, 585, 349, 584, 612, 815, 27, 795, 566, 0, 31, 496, 236, 400, 697, 237, 581, 74, 610, 217, 215, 226, 55, 741, 413, 109, 200, 238, 879, 1066, 811, 460, 671, 0, 253, 1, 1092, 32, 110, 1093, 39, 189, 692, 1000, 640, 929, 183, 422, 269, 67, 537, 243, 106, 977, 78, 193, 412, 204, 101, 146, 137, 814, 262, 210, 696, 138, 716, 99, 148, 514, 472, 438, 131, 243, 0, 265, 1029, 260, 197, 673, 5, 681, 277, 342, 250, 952, 228, 1032, 246, 733, 876, 19, 1, 188, 780, 0, 53, 757, 221, 155, 295, 312, 397, 883, 674, 168, 240, 45, 5, 246, 243, 735, 245, 181, 475, 627, 636, 849, 875, 249, 1047, 6, 1001, 750, 244, 691, 570, 192, 857, 32, 228, 0, 516, 732, 9, 159, 1092, 146, 1067, 95, 131, 985, 979, 9, 154, 134, 243, 491, 153, 340, 777, 39, 180, 707, 1, 684, 183, 794, 314, 687, 871, 208, 99, 212, 502, 169, 5, 143, 793, 867, 5, 249, 410, 251, 98, 348, 0, 747, 928, 713, 52, 984, 90, 0, 494, 301, 243, 803, 865, 1079, 825, 132, 576, 779, 194, 517, 683, 715, 854, 0, 187, 229, 322, 279, 103, 106, 298, 417, 420, 600, 818, 398, 611, 665, 968, 86, 430, 0, 249, 929, 383, 986, 1080, 822, 520, 474, 31, 571, 1085, 606, 749, 753, 832, 856, 1044, 1091, 759, 933, 698, 26, 488, 540, 542, 831, 932, 99, 356, 392, 409, 672, 32, 433, 359, 868, 243, 1, 1031, 81, 0, 1, 32, 666, 104, 579, 6, 346, 414, 54, 501, 617, 702, 529, 231, 455, 789, 945, 1075, 157, 969, 84, 819, 25, 142, 589, 520, 715, 723, 243, 791, 32, 441, 196, 950, 38, 255, 973, 274, 190, 460, 206, 318, 140, 427, 411, 282, 364, 0, 859, 483, 0, 1031, 827, 243, 857, 978, 700, 971, 245, 179, 257, 0, 632, 1, 1, 6, 0, 6, 254, 520, 0, 0, 1, 1045, 0, 1004, 0, 32, 292, 809, 6, 118, 180, 227, 905, 6, 20, 0, 155, 406, 482, 0, 215, 407, 740, 0, 668, 41, 129, 266, 267, 421, 487, 489, 649, 772, 781, 808, 835, 925, 663, 385, 94, 459, 801, 982, 0, 202, 254, 394, 539, 393, 0, 167, 541, 612, 0, 463, 0, 37, 205, 207, 316, 317, 399, 441, 474, 497, 667, 706, 850, 6, 22, 224, 428, 956, 0, 243, 5, 0, 980, 0, 0, 0, 6, 295, 448, 1020, 1, 0, 682, 1003, 309, 333, 787, 441, 0, 441, 944, 441, 1045, 0, 300, 441, 615, 848, 1084, 441, 612, 0, 6, 254, 441, 612, 0, 441, 612, 1007, 0, 478, 441, 1052, 0, 485, 0, 419, 535, 612, 1045, 0, 441, 612, 734, 0, 962, 36, 209, 211, 276, 321, 341, 425, 543, 764, 830, 917, 6, 144, 591, 0, 939, 612, 163, 307, 0, 979, 0, 23, 0, 61, 0, 376, 374, 726, 1, 0, 0, 1, 0, 353, 335, 693, 11, 112, 5, 758, 148, 146, 108, 125, 712, 116, 53, 940, 602, 48, 271, 998, 250, 855, 810, 228, 927, 386, 384, 896, 886, 360, 7, 61, 218, 219, 225, 650, 972, 215, 155, 146, 1063, 748, 145, 423, 153, 149, 965, 754, 143, 745, 5, 693, 547, 203, 788, 796, 766, 368, 375, 890, 574, 572, 249, 243, 245, 250, 995, 261, 1068, 111, 117, 245, 548, 593, 804, 705, 5, 351, 661, 533, 565, 582, 456, 461, 912, 60, 60, 996, 60, 243, 239, 242, 232, 13, 1033, 61, 467, 61, 249, 330, 327, 303, 782, 881, 290, 872, 12, 357, 355, 153, 5, 270, 965, 453, 505, 504, 652, 853, 685, 88, 915, 480, 495, 19, 631, 436, 434, 762, 765, 180, 664, 656, 657, 658, 742, 5, 147, 144, 153, 738, 155, 162, 151, 574, 992, 193, 275, 678, 0, 250, 1046, 587, 216, 695, 1071, 590, 243, 477, 183, 1064, 377, 402, 395, 256, 379, 388, 0, 897, 894, 370, 418, 641, 510, 464, 441, 0, 61, 844, 1027, 246, 824, 852, 953, 35, 195, 1022, 792, 28, 884, 344, 343, 333, 336, 332, 693, 695, 560, 1017, 551, 934, 564, 1069, 563, 347, 717, 719, 721, 141, 730, 1035, 142, 644, 732, 731, 498, 499, 485, 1, 19, 490, 916, 503, 623, 628, 387, 621, 635, 889, 1052, 324, 293, 273, 311, 396, 310, 75, 243, 709, 710, 846, 874, 155, 520, 880, 891, 127, 380, 381, 444, 898, 241, 252, 350, 373, 484, 559, 821, 861, 914, 926, 1054, 372, 603, 405, 368, 0, 441, 458, 442, 909, 1021, 143, 553, 143, 441, 588, 813, 228, 233, 198, 1038, 511, 61, 727, 743, 415, 612, 878, 922, 29, 930, 1009, 526, 942, 1008, 268, 826, 999, 751, 770, 816, 292, 676, 763, 1042, 0, 979, 768, 0, 1, 468, 799, 338, 800, 1042, 0, 91, 0, 462, 910, 913, 989, 182, 451, 452, 479, 820, 919, 447, 446, 441, 656, 0, 441, 9, 441, 315, 328, 329, 507, 325, 320, 313, 612, 1045, 877, 308, 191, 304, 302, 299, 297, 5, 10, 840, 7, 509, 21, 61, 633, 32, 612, 1, 989, 1, 897, 367, 527, 596, 597, 598, 599, 46, 378, 408, 28, 81, 521, 520, 642, 643, 523, 612, 923, 924, 29, 29, 1019, 29, 839, 29, 508, 29, 81, 293, 963, 608, 609, 704, 737, 1001, 264, 435, 432, 431, 653, 918, 289, 443, 441, 449, 1082, 760, 549, 955, 707, 708, 1088, 1048, 870, 5, 558, 500, 481, 243, 520, 243, 802, 249, 528, 6, 853, 6, 720, 6, 565, 1, 34, 858, 1052, 32, 24, 7, 126, 1077, 5, 366, 713, 59, 713, 488, 693, 0, 0, 937, 0, 40, 773, 965, 0, 5, 699, 842, 550, 807, 806, 823, 693, 701, 530, 577, 1070, 474, 19, 186, 755, 780, 887, 817, 0, 6, 0, 9, 32, 520, 0, 1045, 0, 366, 0, 81, 366, 441, 869, 899, 994, 0, 761, 1, 880, 902, 0, 612, 1014, 0, 441, 612, 0, 441, 612, 1012, 0, 441, 612, 847, 0, 441, 613, 1052, 612, 903, 1013, 0, 143, 441, 943, 0, 967, 605, 50, 812, 552, 7, 61, 218, 218, 1055, 5, 218, 836, 1081, 752, 991, 254, 445, 165, 1039, 61, 243, 473, 951, 736, 245, 0, 843, 250, 250, 1086, 863, 859, 258, 837, 834, 245, 243, 1024, 853, 334, 959, 424, 767, 960, 961, 49, 921, 1073, 1074, 1060, 0, 583, 966, 0, 295, 349, 965, 0, 485, 19, 32, 19, 446, 612, 1052, 199, 214, 293, 337, 390, 554, 675, 797, 291, 874, 892, 371, 382, 991, 0, 81, 0, 656, 612, 904, 0, 81, 441, 441, 612, 146, 904, 1045, 296, 938, 598, 948, 297, 523, 941, 524, 614, 612, 1045, 1087, 525, 29, 612, 731, 976, 32, 243, 39, 243, 0, 243, 146, 713, 1, 107, 920, 488, 60, 488, 297, 488, 0, 1, 0, 32, 0, 893, 571, 690, 856, 0, 441, 941, 0, 441, 612, 769, 0, 441, 612, 1009, 1045, 7, 60, 7, 64, 243, 965, 19, 441, 656, 904, 81, 441, 0, 9, 441, 612, 0, 441, 612, 297, 523, 1019, 523, 941, 441, 612, 1043, 941, 612, 57, 841, 259, 864, 245, 1002, 243, 993, 829, 285, 619, 622, 625, 618, 624, 1049, 177, 612, 904, 1045, 964, 1005, 366, 888, 679, 936, 1036, 641, 1057, 655, 988, 987, 957, 1058, 930, 8, 907, 1051, 1052, 0, 441, 612, 901, 6, 1062, 978, 979, 1028, 997, 991, 1024, 990, 1023, 1016, 1018, 562, 1076, 1078, 1015, 1053, 29, 941, 0, 9, 1025, 1026, 860, 1090, 1061, 1043, 1083, 15, 15}; 
 