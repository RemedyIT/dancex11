#--------------------------------------------------------------------
# @file    require.rb
# @author  Martin Corino
#
# @brief   BRIX11 DANCEX11 brix collection loader
#
# @copyright Copyright (c) Remedy IT Expertise BV
#--------------------------------------------------------------------

module BRIX11

  module DANCEX11

    ROOT = File.dirname(__FILE__)
    TITLE = 'DANCEX11'.freeze
    DESC = 'BRIX11 DANCEX11 brix collection'.freeze
    COPYRIGHT = "Copyright (c) 2014-#{Time.now.year} Remedy IT Expertise BV, The Netherlands".freeze
    VERSION = {major: 0, minor: 1, release: 0}

    Collection.configure(:dancex11, ROOT, TITLE, DESC, COPYRIGHT, VERSION) do |cfg|

      # declare dependency on :ciaox11 collection (forces it to load first)
      cfg.add_collection(:ciaox11)

      cfg.on_setup do |optparser, options|
        if Exec.mswin?
          search_path_env = 'PATH'
        else
          search_path_env = 'LD_LIBRARY_PATH'
        end
        # define common environment for spawning BRIX11 subprocesses
        dancex11_root = Exec.get_run_environment('DANCEX11_ROOT')
        dancex11_root ||= Exec.update_run_environment('DANCEX11_ROOT', File.dirname(File.dirname(DANCEX11::ROOT)))

        # should we prepare for crossbuilds?
        if BRIX11.options.config.crossbuild
          x11_host_root = (Exec.get_run_environment('X11_HOST_ROOT') || '')
          # update library search paths for host (tool) libs
          Exec.update_run_environment(search_path_env, File.join(x11_host_root, 'dancex11', 'lib'), :append)
          # update executable search path for host deployment tools
          Exec.update_run_environment('PATH', File.join(x11_host_root, 'dancex11', 'bin'), :prepend)
        else # or regular build environment
          # update library search paths for (tool) libs
          Exec.update_run_environment(search_path_env, File.join(dancex11_root, 'lib'), :append)
          # update executable search path for deployment tools
          Exec.update_run_environment('PATH', File.join(dancex11_root, 'bin'), :prepend)
        end

        # update documentation config
        Common::GenerateDocumentation::OPTIONS[:docsources].merge!({
            'dancex11'  => ['${DANCEX11_ROOT}/docs']
          })
        Common::GenerateDocumentation::OPTIONS[:adoc_attribs]['dancex11_src_root'] = Exec.get_run_environment('DANCEX11_ROOT')
      end

    end # Collection.configure

  end # DANCEX11

end # BRIX11
