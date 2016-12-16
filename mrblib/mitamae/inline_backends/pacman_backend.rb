module MItamae
  module InlineBackends
    class PacmanBackend
      def initialize
        # FIXME: Detect RootDir and DBPath
        @handle = Alpm::Handle.new('/', '/var/lib/pacman')
      rescue => e
        MItamae.logger.warn("Failed to initialize alpm: #{e.class}: #{e.message}")
        @handle = nil
      end

      def runnable?(type, *args)
        @handle && respond_to?(type)
      end

      def run(type, *args)
        send(type, *args)
      end

      def check_package_is_installed(name, version = nil)
        @handle.installed?(name) || @handle.group_installed?(name)
      end

      def get_package_version(name)
        Specinfra::CommandResult.new(stdout: @handle.installed_version(name), stderr: '', exit_status: 0)
      end
    end
  end
end
