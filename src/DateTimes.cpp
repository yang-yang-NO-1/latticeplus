#include "DateTimes.h"

DateTimes::DateTimes()
{
  rtc.RX8025_init();
  dht11 = SimpleDHT11(16);
}

/**
 * @brief 获取时间信息
 *
 * @return Times
 */
Times DateTimes::getTimes()
{
  times.s = rtc.getSecond();
  times.m = rtc.getMinute();
  times.h = rtc.getHour();
  // 如果从芯片取到的小时数据为25的时候,表示这个数据不是从芯片里面取到的,所以就需要换成我们手动的
  // if (times.h == 25)//此处修改20250128
    if (1)//此处修改20250128
  {
    datetime = DateTime(currtimestamp);
    times.h = datetime.hour();
    times.m = datetime.minute();
    times.s = datetime.second();
  }
  return times;
}

/**
 * @brief 获取日期信息
 *
 * @return Dates
 */
Dates DateTimes::getDates()
{
  dates.y = 2000 + rtc.getYear();
  dates.m = rtc.getMonth();
  dates.d = rtc.getDate();
  // 如果从芯片取到的年数据为85的时候,表示这个数据不是从芯片里面取到的,所以就需要换成我们手动的
  // if (dates.m == 85)//此处修改20250128
  if (1)
  {
    datetime = DateTime(currtimestamp);
    dates.y = datetime.year();
    dates.m = datetime.month();
    dates.d = datetime.day();
  }
  return dates;
}

/**
 * @brief 获取温度信息
 *
 * @return int
 */
int DateTimes::getTemperature()
{
  // 如果从芯片取到的年数据为85的时候,表示这个数据不是从芯片里面取到的,所以就需要换成我们手动的
  // if (ds3231.getMonth(century) == 85)
  // {
    if (System::is_overtime(3000)) // 这里三秒获取一次温度信息
    {
      if (dht11.read(&temperature, &humidity, NULL) != SimpleDHTErrSuccess)
      {
        // 这里表示获取温度失败,暂时不做任何处理
      }
    }
    return temperature * 100;
  // }
  // return ds3231.getTemperature() * 100;
}

/**
 * @brief 获取湿度信息
 *
 * @return int
 */
int DateTimes::getHumidity()
{
  if (System::is_overtime(3000)) // 这里三秒获取一次温度信息
    {
      if (dht11.read(&temperature, &humidity, NULL) != SimpleDHTErrSuccess)
      {
        // 这里表示获取温度失败,暂时不做任何处理
      }
    }
    return humidity * 100;
}

/**
 * @brief 给时钟芯片和系统时间设置时间信息
 *
 * @param timestamp
 */
void DateTimes::setDateTimes(long timestamp)
{
  currtimestamp = timestamp;
  datetime = DateTime(timestamp);
  rtc.setRtcTime(datetime.second(),datetime.minute(),datetime.hour(),datetime.dayOfTheWeek(),datetime.day(),datetime.month(),datetime.year() % 100);
  Serial.println(datetime.dayOfTheWeek());
}

/**
 * @brief 获取时间戳信息
 *
 * @return long
 */
long DateTimes::getTimestamp()
{
  // 如果从芯片取到的年数据为85的时候,表示这个数据不是从芯片里面取到的,所以就需要换成我们手动的
  if (rtc.getMonth() == 85)
  {
    return currtimestamp;
  }
  return rtc.getUnixtime();
}

/**
 * @brief 系统时间戳++
 *
 */
void DateTimes::timestampAdd()
{
  currtimestamp++;
}

/**
 * @brief 保存倒计时时间戳信息
 *
 * @param timestamp
 */
void DateTimes::saveCountdownTimestamp(long timestamp)
{
  uint8_t arr[5];
  for (int i = 0; i < 5; i++)
  {
    arr[i] = (timestamp & 0xff);
    timestamp >>= 8;
  }
  EEPROMTool.saveData(arr, COUNTDOWN_TIME, 5);
}

/**
 * @brief 获取倒计时时间戳信息
 *
 * @return long
 */
long DateTimes::getCountdownTimestamp()
{
  long timestamp = 0;
  uint8_t *temp = EEPROMTool.loadData(COUNTDOWN_TIME, 5);
  for (int i = 0; i < 5; i++)
  {
    timestamp += temp[i] << (i * 8);
  }
  // 用完以后删除内存
  free(temp);
  return timestamp;
}
