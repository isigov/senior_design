/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include <iostream>
#include <fstream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include "ue.h"
#include "metrics_stdout.h"
#include "metrics_csv.h"
#include "srslte/common/metrics_hub.h"
#include "srslte/version.h"

using namespace std;
using namespace srsue;
namespace bpo = boost::program_options;

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
string config_file;

void parse_args(all_args_t *args, int argc, char *argv[]) {

  // Command line only options
  bpo::options_description general("General options");

  general.add_options()
    ("help,h", "Produce help message")
    ("version,v", "Print version information and exit");

  // Command line or config file options
  bpo::options_description common("Configuration options");
  common.add_options()
    ("rf.dl_earfcn", bpo::value<uint32_t>(&args->rf.dl_earfcn)->default_value(3400), "Downlink EARFCN")
    ("rf.freq_offset", bpo::value<float>(&args->rf.freq_offset)->default_value(0), "(optional) Frequency offset")
    ("rf.rx_gain", bpo::value<float>(&args->rf.rx_gain)->default_value(-1), "Front-end receiver gain")
    ("rf.tx_gain", bpo::value<float>(&args->rf.tx_gain)->default_value(-1), "Front-end transmitter gain")
    ("rf.nof_rx_ant", bpo::value<uint32_t>(&args->rf.nof_rx_ant)->default_value(1), "Number of RX antennas")

    ("rf.device_name", bpo::value<string>(&args->rf.device_name)->default_value("auto"), "Front-end device name")
    ("rf.device_args", bpo::value<string>(&args->rf.device_args)->default_value("auto"), "Front-end device arguments")
    ("rf.time_adv_nsamples", bpo::value<string>(&args->rf.time_adv_nsamples)->default_value("auto"),
     "Transmission time advance")
    ("rf.burst_preamble_us", bpo::value<string>(&args->rf.burst_preamble)->default_value("auto"),
     "Transmission time advance")

    ("pcap.enable", bpo::value<bool>(&args->pcap.enable)->default_value(false),
     "Enable MAC packet captures for wireshark")
    ("pcap.filename", bpo::value<string>(&args->pcap.filename)->default_value("ue.pcap"), "MAC layer capture filename")

    ("trace.enable", bpo::value<bool>(&args->trace.enable)->default_value(false), "Enable PHY and radio timing traces")
    ("trace.phy_filename", bpo::value<string>(&args->trace.phy_filename)->default_value("ue.phy_trace"),
     "PHY timing traces filename")
    ("trace.radio_filename", bpo::value<string>(&args->trace.radio_filename)->default_value("ue.radio_trace"),
     "Radio timing traces filename")

    ("gui.enable", bpo::value<bool>(&args->gui.enable)->default_value(false), "Enable GUI plots")

    ("log.phy_level", bpo::value<string>(&args->log.phy_level), "PHY log level")
    ("log.phy_hex_limit", bpo::value<int>(&args->log.phy_hex_limit), "PHY log hex dump limit")
    ("log.mac_level", bpo::value<string>(&args->log.mac_level), "MAC log level")
    ("log.mac_hex_limit", bpo::value<int>(&args->log.mac_hex_limit), "MAC log hex dump limit")
    ("log.rlc_level", bpo::value<string>(&args->log.rlc_level), "RLC log level")
    ("log.rlc_hex_limit", bpo::value<int>(&args->log.rlc_hex_limit), "RLC log hex dump limit")
    ("log.pdcp_level", bpo::value<string>(&args->log.pdcp_level), "PDCP log level")
    ("log.pdcp_hex_limit", bpo::value<int>(&args->log.pdcp_hex_limit), "PDCP log hex dump limit")
    ("log.rrc_level", bpo::value<string>(&args->log.rrc_level), "RRC log level")
    ("log.rrc_hex_limit", bpo::value<int>(&args->log.rrc_hex_limit), "RRC log hex dump limit")
    ("log.gw_level", bpo::value<string>(&args->log.gw_level), "GW log level")
    ("log.gw_hex_limit", bpo::value<int>(&args->log.gw_hex_limit), "GW log hex dump limit")
    ("log.nas_level", bpo::value<string>(&args->log.nas_level), "NAS log level")
    ("log.nas_hex_limit", bpo::value<int>(&args->log.nas_hex_limit), "NAS log hex dump limit")
    ("log.usim_level", bpo::value<string>(&args->log.usim_level), "USIM log level")
    ("log.usim_hex_limit", bpo::value<int>(&args->log.usim_hex_limit), "USIM log hex dump limit")


    ("log.all_level", bpo::value<string>(&args->log.all_level)->default_value("info"), "ALL log level")
    ("log.all_hex_limit", bpo::value<int>(&args->log.all_hex_limit)->default_value(32), "ALL log hex dump limit")

    ("log.filename", bpo::value<string>(&args->log.filename)->default_value("/tmp/ue.log"), "Log filename")

    ("usim.algo", bpo::value<string>(&args->usim.algo), "USIM authentication algorithm")
    ("usim.op", bpo::value<string>(&args->usim.op), "USIM operator variant")
    ("usim.amf", bpo::value<string>(&args->usim.amf), "USIM authentication management field")
    ("usim.imsi", bpo::value<string>(&args->usim.imsi), "USIM IMSI")
    ("usim.imei", bpo::value<string>(&args->usim.imei), "USIM IMEI")
    ("usim.k", bpo::value<string>(&args->usim.k), "USIM K")


    /* Expert section */
    ("expert.phy.worker_cpu_mask",
     bpo::value<int>(&args->expert.phy.worker_cpu_mask)->default_value(-1),
     "cpu bit mask (eg 255 = 1111 1111)")

    ("expert.phy.sync_cpu_affinity",
     bpo::value<int>(&args->expert.phy.sync_cpu_affinity)->default_value(-1),
     "index of the core used by the sync thread")

    ("expert.ue_category",
     bpo::value<string>(&args->expert.ue_cateogry)->default_value("4"),
     "UE Category (1 to 5)")

    ("expert.metrics_period_secs",
     bpo::value<float>(&args->expert.metrics_period_secs)->default_value(1.0),
     "Periodicity for metrics in seconds")

    ("expert.metrics_csv_enable",
     bpo::value<bool>(&args->expert.metrics_csv_enable)->default_value(false),
     "Write UE metrics to CSV file")

    ("expert.metrics_csv_filename",
     bpo::value<string>(&args->expert.metrics_csv_filename)->default_value("/tmp/ue_metrics.csv"),
     "Metrics CSV filename")

    ("expert.pregenerate_signals",
     bpo::value<bool>(&args->expert.pregenerate_signals)->default_value(false),
     "Pregenerate uplink signals after attach. Improves CPU performance.")

    ("expert.rssi_sensor_enabled",
     bpo::value<bool>(&args->expert.phy.rssi_sensor_enabled)->default_value(true),
     "Enable or disable RF frontend RSSI sensor. In some USRP devices can cause segmentation fault")

    ("expert.prach_gain",
     bpo::value<float>(&args->expert.phy.prach_gain)->default_value(-1.0),
     "Disable PRACH power control")

    ("expert.cqi_max",
     bpo::value<int>(&args->expert.phy.cqi_max)->default_value(15),
     "Upper bound on the maximum CQI to be reported. Default 15.")

    ("expert.cqi_fixed",
     bpo::value<int>(&args->expert.phy.cqi_fixed)->default_value(-1),
     "Fixes the reported CQI to a constant value. Default disabled.")

    ("expert.snr_ema_coeff",
     bpo::value<float>(&args->expert.phy.snr_ema_coeff)->default_value(0.1),
     "Sets the SNR exponential moving average coefficient (Default 0.1)")

    ("expert.snr_estim_alg",
     bpo::value<string>(&args->expert.phy.snr_estim_alg)->default_value("refs"),
     "Sets the noise estimation algorithm. (Default refs)")

    ("expert.pdsch_max_its",
     bpo::value<int>(&args->expert.phy.pdsch_max_its)->default_value(4),
     "Maximum number of turbo decoder iterations")

    ("expert.attach_enable_64qam",
     bpo::value<bool>(&args->expert.phy.attach_enable_64qam)->default_value(false),
     "PUSCH 64QAM modulation before attachment")

    ("expert.nof_phy_threads",
     bpo::value<int>(&args->expert.phy.nof_phy_threads)->default_value(2),
     "Number of PHY threads")

    ("expert.equalizer_mode",
     bpo::value<string>(&args->expert.phy.equalizer_mode)->default_value("mmse"),
     "Equalizer mode")

    ("expert.cfo_integer_enabled",
     bpo::value<bool>(&args->expert.phy.cfo_integer_enabled)->default_value(false),
     "Enables integer CFO estimation and correction.")

    ("expert.cfo_correct_tol_hz",
     bpo::value<float>(&args->expert.phy.cfo_correct_tol_hz)->default_value(50.0),
     "Tolerance (in Hz) for digial CFO compensation.")

    ("expert.time_correct_period",
     bpo::value<int>(&args->expert.phy.time_correct_period)->default_value(5),
     "Period for sampling time offset correction.")

    ("expert.sfo_correct_disable",
     bpo::value<bool>(&args->expert.phy.sfo_correct_disable)->default_value(false),
     "Disables phase correction before channel estimation.")

    ("expert.sss_algorithm",
     bpo::value<string>(&args->expert.phy.sss_algorithm)->default_value("full"),
     "Selects the SSS estimation algorithm.")

    ("expert.estimator_fil_w",
     bpo::value<float>(&args->expert.phy.estimator_fil_w)->default_value(0.1),
     "Chooses the coefficients for the 3-tap channel estimator centered filter.")


    ("rf_calibration.tx_corr_dc_gain", bpo::value<float>(&args->rf_cal.tx_corr_dc_gain)->default_value(0.0),
     "TX DC offset gain correction")
    ("rf_calibration.tx_corr_dc_phase", bpo::value<float>(&args->rf_cal.tx_corr_dc_phase)->default_value(0.0),
     "TX DC offset phase correction")
    ("rf_calibration.tx_corr_iq_i", bpo::value<float>(&args->rf_cal.tx_corr_iq_i)->default_value(0.0),
     "TX IQ imbalance inphase correction")
    ("rf_calibration.tx_corr_iq_q", bpo::value<float>(&args->rf_cal.tx_corr_iq_q)->default_value(0.0),
     "TX IQ imbalance quadrature correction");

  // Positional options - config file location
  bpo::options_description position("Positional options");
  position.add_options()
    ("config_file", bpo::value<string>(&config_file), "UE configuration file");
  bpo::positional_options_description p;
  p.add("config_file", -1);

  // these options are allowed on the command line
  bpo::options_description cmdline_options;
  cmdline_options.add(common).add(position).add(general);

  // parse the command line and store result in vm
  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
  bpo::notify(vm);

  // help option was given - print usage and exit
  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
    cout << common << endl << general << endl;
    exit(0);
  }

  // print version number and exit
  if (vm.count("version")) {
    cout << "Version " <<
         srslte_get_version_major() << "." <<
         srslte_get_version_minor() << "." <<
         srslte_get_version_patch() << endl;
    exit(0);
  }

  // no config file given - print usage and exit
  if (!vm.count("config_file")) {
    cout << "Error: Configuration file not provided" << endl;
    cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
    exit(0);
  } else {
    cout << "Reading configuration file " << config_file << "..." << endl;
    ifstream conf(config_file.c_str(), ios::in);
    if (conf.fail()) {
      cout << "Failed to read configuration file " << config_file << " - exiting" << endl;
      exit(1);
    }
    bpo::store(bpo::parse_config_file(conf, common), vm);
    bpo::notify(vm);
  }

  // Apply all_level to any unset layers
  if (vm.count("log.all_level")) {
    if (!vm.count("log.phy_level")) {
      args->log.phy_level = args->log.all_level;
    }
    if (!vm.count("log.mac_level")) {
      args->log.mac_level = args->log.all_level;
    }
    if (!vm.count("log.rlc_level")) {
      args->log.rlc_level = args->log.all_level;
    }
    if (!vm.count("log.pdcp_level")) {
      args->log.pdcp_level = args->log.all_level;
    }
    if (!vm.count("log.rrc_level")) {
      args->log.rrc_level = args->log.all_level;
    }
    if (!vm.count("log.nas_level")) {
      args->log.nas_level = args->log.all_level;
    }
    if (!vm.count("log.gw_level")) {
      args->log.gw_level = args->log.all_level;
    }
    if (!vm.count("log.usim_level")) {
      args->log.usim_level = args->log.all_level;
    }
  }


  // Apply all_hex_limit to any unset layers
  if (vm.count("log.all_hex_limit")) {
    if (!vm.count("log.phy_hex_limit")) {
      args->log.phy_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.mac_hex_limit")) {
      args->log.mac_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.rlc_hex_limit")) {
      args->log.rlc_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.pdcp_hex_limit")) {
      args->log.pdcp_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.rrc_hex_limit")) {
      args->log.rrc_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.nas_hex_limit")) {
      args->log.nas_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.gw_hex_limit")) {
      args->log.gw_hex_limit = args->log.all_hex_limit;
    }
    if (!vm.count("log.usim_hex_limit")) {
      args->log.usim_hex_limit = args->log.all_hex_limit;
    }
  }
}

static bool running = true;
static bool do_metrics = false;
metrics_stdout metrics_screen;

void sig_int_handler(int signo) {
  running = false;
}

void *input_loop(void *m) {
  char key;
  while (running) {
    cin >> key;
    if ('t' == key) {
      do_metrics = !do_metrics;
      if (do_metrics) {
        cout << "Enter t to stop trace." << endl;
      } else {
        cout << "Enter t to restart trace." << endl;
      }
      metrics_screen.toggle_print(do_metrics);
    }
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  srslte::metrics_hub<ue_metrics_t> metricshub;
  signal(SIGINT, sig_int_handler);
  all_args_t args;
  parse_args(&args, argc, argv);

  srsue_instance_type_t type = LTE;
  ue_base *ue = ue_base::get_instance(type);
  if (!ue) {
    cout << "Error creating UE instance." << endl << endl;
    exit(1);
  }

  cout << "---  Software Radio Systems " << srsue_instance_type_text[type] << " UE  ---" << endl << endl;
  if (!ue->init(&args)) {
    exit(1);
  }

  metricshub.init(ue, args.expert.metrics_period_secs);
  metricshub.add_listener(&metrics_screen);
  metrics_screen.set_ue_handle(ue);

  metrics_csv metrics_file(args.expert.metrics_csv_filename);
  if (args.expert.metrics_csv_enable) {
    metricshub.add_listener(&metrics_file);
    metrics_file.set_ue_handle(ue);
  }

  pthread_t input;
  pthread_create(&input, NULL, &input_loop, &args);

  bool plot_started = false;
  bool signals_pregenerated = false;
  while (running) {
    if (ue->is_attached()) {
      if (!signals_pregenerated && args.expert.pregenerate_signals) {
        ue->pregenerate_signals(true);
        signals_pregenerated = true;
      }
      if (!plot_started && args.gui.enable) {
        ue->start_plot();
        plot_started = true;
      }
    }
    sleep(1);
  }
  pthread_cancel(input);
  metricshub.stop();
  ue->stop();
  ue->cleanup();
  cout << "---  exiting  ---" << endl;
  exit(0);
}
