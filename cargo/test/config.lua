sys = {
	devices = {
		--svd0 = {
		--type = "svd",
		--max_size_log = 15,
		--},
		svd0 = {
			type = "svd-sync",
			-- synthesizable and thus doesn't need configureable max size
		},
	},
}
