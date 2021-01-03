#ifndef bt_Position_hpp
#define bt_Position_hpp

#include <analyze_prices.hpp>
#include <PositionStats.hpp>

#include <iostream>

namespace bt {

struct ShouldBuyResult {
  bool buy = false;
  float buyPrice = 0.0;
};

struct ShouldSellResult {
  bool sell = false;
  float sellPrice = 0.0;
};


class Position
{
public:
  Position(const std::string& nm,
           float bal = 10000.0,
           float buyPrc = 0.0,
           unsigned numShrs = 0)
  : invested(false), balance(bal), buyPrice(buyPrc),
    numShares(numShrs), previousPrice(0.0),
    numProfitableSales(0), numTotalSales(0),
    currentDate(""), smaPreviousClose(0.0),
    isOpeningPrice(true),name(nm),
    daysSinceSell(2),
    positionStats()
  {}

  virtual ~Position(){}

  virtual void set_current_date(const std::string& date,
                                float smaAtPreviousClose)
  {
    currentDate = date;
    smaPreviousClose = smaAtPreviousClose;
    isOpeningPrice = true;
    if (!is_invested()) {
      ++daysSinceSell;
    }
  }

  void process_price(float price);

  const std::string& get_name() const { return name; }
  bool is_invested() const { return invested; }
  float get_balance() const;
  float get_buy_price() const { return buyPrice; }
  unsigned get_num_shares() const { return numShares; }
  float get_sma_at_previous_close() const { return smaPreviousClose; }
  float get_previous_price() const { return previousPrice; }

  const PositionStats& get_position_stats() const
  { return positionStats; }

private:
  virtual ShouldBuyResult should_buy(float price, bool isOpeningPrice) = 0;

  virtual ShouldSellResult should_sell(float price, bool isOpeningPrice) = 0;

  void buy(float price);
  void sell(float price);

  bool invested = false;
  float balance = 10000.0;
  float buyPrice = 0.0;
  unsigned numShares = 0;
  float previousPrice = 0.0;
  unsigned numProfitableSales = 0;
  unsigned numTotalSales = 0;

  std::string currentDate;
  float smaPreviousClose = 0.0;
  bool isOpeningPrice = true;
  std::string name;
  unsigned daysSinceSell = 2;
  PositionStats positionStats;
};

class MyPosition : public Position
{
public:
  MyPosition(float trailingPercent)
  : Position("MyPosition"),
    trailingStopPercent(trailingPercent)
  {}

  ~MyPosition(){}

private:
  ShouldBuyResult should_buy(float price, bool isOpeningPrice) override;

  ShouldSellResult should_sell(float price, bool isOpeningPrice) override;

  bool openingPriceBelowSma = false;
  float movingPeak = 0.0;
  float trailingStopPercent = 0.0;
};

class BuyAndHoldPosition : public Position
{
public:
  BuyAndHoldPosition(const std::string& equityName)
  : Position(equityName)
  {}

  ~BuyAndHoldPosition(){}

private:
  ShouldBuyResult should_buy(float price, bool /*isOpeningPrice*/) override
  {
    if (is_invested()) {
      return {false, price};
    }
    //I want to buy at the first closing price, which is the
    //fourth price we receive...
    static unsigned numPrices = 0;
    ++numPrices;
    if (numPrices == 4) {
      return {true, price};
    }
    return {false, 0.0};
  }

  ShouldSellResult should_sell(float price, bool /*isOpeningPrice*/) override {
    return {false, price};
  }
};

class SMAPosition : public Position
{
public:
  SMAPosition() : Position("SMASignal") {}
  virtual ~SMAPosition() {}

private:
  virtual ShouldBuyResult should_buy(float price, bool isOpeningPrice);

  virtual ShouldSellResult should_sell(float price, bool isOpeningPrice);
};

class TSPPosition : public Position
{
public:
  TSPPosition(float trailPercentSell, float trailPercentBuy=-1.0)
  : Position("TSPSignal"),
    trailingStopPercentSell(trailPercentSell),
    trailingStopPercentBuy((trailPercentBuy<0.0 ? trailPercentSell : trailPercentBuy)),
    trailPrice(-1.0), movingPeak(-1.0)
  {}
  virtual ~TSPPosition() {}

private:
  virtual ShouldBuyResult should_buy(float price, bool isOpeningPrice);

  virtual ShouldSellResult should_sell(float price, bool isOpeningPrice);

  float trailingStopPercentSell = 0.0;
  float trailingStopPercentBuy = 0.0;
  float trailPrice = -1.0;
  float movingPeak = -1.0;
};

inline
std::ostream& operator<<(std::ostream& os, const Position& pos)
{
  os << "{buyPrice="<<pos.get_buy_price()<<" numShares="<<pos.get_num_shares()
     << " balance="<<pos.get_balance()<<"}";
  return os;
}

} // namespace bt
#endif
