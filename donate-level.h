#pragma once

/*
 * Dev donation.
 * Percentage of your hashing power that you want to donate to the developer, can be 0.0 if you don't want to do that.
 * Example of how it works for the default setting of 2.0:
 * You miner will mine into your usual pool for 98 minutes, then switch to the developer's pool for 2.0 minute.
 * Switching is instant, and only happens after a successful connection, so you never loose any hashes.
 *
 * If you plan on changing this setting to 0.0 please consider making a one off donation to our wallets:
 * fireice-uk:
 * 48SxhDUBJrW21FWx7tiQtx8FGF46P8WhhZ2owmtouXLhAKSZviFxR54MyJTijfeJog4CdMJyBU17kbAHfJj6nPFYAHBCzPV
 * psychocrypt:
 * 48SxhDUBJrW21FWx7tiQtx8FGF46P8WhhZ2owmtouXLhAKSZviFxR54MyJTijfeJog4CdMJyBU17kbAHfJj6nPFYAHBCzPV
 *
 */

constexpr double fDevDonationLevel = 2.0 / 100.0;
